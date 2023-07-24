#include <srcdiff_single.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_comment.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_compare.hpp>
#include <shortest_edit_script.h>

#include <cstring>
#include <map>
#include <list>

const std::string convert("convert");
const srcml_node::srcml_attr diff_convert_type(DIFF_TYPE, convert);

srcdiff_single::srcdiff_single(const srcdiff_diff & diff, unsigned int start_original, unsigned int start_modified) : srcdiff_diff(diff), start_original(start_original), start_modified(start_modified) {}

static std::list<srcml_node::srcml_attr> merge_properties(const std::list<srcml_node::srcml_attr> & properties_original, const std::list<srcml_node::srcml_attr> & properties_modified) {

  std::list<srcml_node::srcml_attr> attributes;

  auto citr_original = properties_original.begin();
  auto citr_modified = properties_modified.begin();

  while(citr_original != properties_original.end() && citr_modified != properties_modified.end()) {

    if(citr_original->name == citr_modified->name) {

      if(*citr_original->value == citr_modified->value) {
        attributes.emplace_back(citr_original->name, *citr_original->value);
      }
      else {
        attributes.emplace_back(citr_original->name, *citr_original->value + std::string("|") +*citr_modified->value);
      }

      ++citr_original;
      ++citr_modified;

    } else {

      ++citr_modified;

      bool is_end = citr_modified == properties_modified.end();
      std::string name;
      if(!is_end) name = citr_modified->name;

      --citr_modified;

      if(!is_end && citr_original->name == name) {

        attributes.emplace_back(citr_modified->name, std::string("|") + *citr_modified->value);

        ++citr_modified;

      } else {

        attributes.emplace_back(citr_original->name, *citr_original->value + std::string("|"));

        ++citr_original;

      }

    }

  }

  while(citr_original != properties_original.end()) {

      attributes.emplace_back(citr_original->name, *citr_original->value + std::string("|"));

      ++citr_original;

  }

  while(citr_modified != properties_modified.end()) {

      attributes.emplace_back(citr_modified->name, std::string("|") + *citr_modified->value);

      ++citr_modified;

  }

  return attributes;

}

void srcdiff_single::output_recursive_same() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  if(element_list_original.at(start_original).get_root()->is_temporary == element_list_modified.at(start_modified).get_root()->is_temporary) {
    out.output_node(out.diff_common_start, SES_COMMON);
  }

  if(srcdiff_compare::node_compare(element_list_original.at(start_original).get_root(), element_list_modified.at(start_modified).get_root()) == 0) {

    out.output_node(element_list_original.at(start_original).get_root(), element_list_modified.at(start_modified).get_root(), SES_COMMON);

  } else {

    std::shared_ptr<srcml_node> merged_node = std::make_shared<srcml_node>(*element_list_original.at(start_original).get_root());
    merged_node->is_empty = element_list_original.at(start_original).get_root()->is_empty && element_list_modified.at(start_modified).get_root()->is_empty;
    merged_node->properties = merge_properties(element_list_original.at(start_original).get_root()->properties,
                                              element_list_modified.at(start_modified).get_root()->properties);
    out.output_node(merged_node, SES_COMMON);

  }

  ++out.last_output_original();
  ++out.last_output_modified();

  // diff comments differently then source-code
  if(element_list_original.at(start_original).get_root()->name == "comment") {

    // collect subset of nodes
    element_list next_set_original
      = element_list(out.nodes_original(), element_list_original.at(start_original).at(1)
                        , element_list_original.at(start_original).at(element_list_original.at(start_original).size() - 1));

    element_list next_set_modified
      = element_list(out.nodes_modified(), element_list_modified.at(start_modified).at(1)
                        , element_list_modified.at(start_modified).at(element_list_modified.at(start_modified).size() - 1));

    srcdiff_comment diff(out, next_set_original, next_set_modified);
    diff.output();

  } else {

      // collect subset of nodes
      element_list next_set_original(out.nodes_original());
      if(!element_list_original.at(start_original).get_root()->is_empty)
        next_set_original = element_list(out.nodes_original(),
                                      element_list_original.at(start_original).at(1),
                                      element_list_original.at(start_original).back());

      element_list next_set_modified(out.nodes_modified());
      if(!element_list_modified.at(start_modified).get_root()->is_empty)
        next_set_modified = element_list(out.nodes_modified(),
                                      element_list_modified.at(start_modified).at(1),
                                      element_list_modified.at(start_modified).back());

      srcdiff_diff diff(out, next_set_original, next_set_modified);
      diff.output();

  }

  output_common(element_list_original.at(start_original).back() + 1, element_list_modified.at(start_modified).back() + 1);

  if(element_list_original.at(start_original).get_root()->is_temporary == element_list_modified.at(start_modified).get_root()->is_temporary) {
    out.output_node(out.diff_common_end, SES_COMMON);
  }

}

void srcdiff_single::output_recursive_interchangeable() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  const std::shared_ptr<srcml_node> & original_start_node = element_list_original.at(start_original).get_root();
  const std::shared_ptr<srcml_node> & modified_start_node = element_list_modified.at(start_modified).get_root();

  int original_collect_start_pos = 1;
  if(original_start_node->name == "if_stmt") {
    // must have if, if interchange passed
    while(out.nodes_original().at(element_list_original.at(start_original).at(original_collect_start_pos))->name != "if") {
      ++original_collect_start_pos;
    }
    ++original_collect_start_pos;
  }

  int modified_collect_start_pos = 1;
  if(modified_start_node->name == "if_stmt") {
    // must have if, if interchange passed
    while(out.nodes_modified().at(element_list_modified.at(start_modified).at(modified_collect_start_pos))->name != "if") {
      ++modified_collect_start_pos;
    }
    ++modified_collect_start_pos;
  }

  // get keyword if present
  const std::shared_ptr<srcml_node> & keyword_node_original = out.nodes_original().at(element_list_original.at(start_original).at(original_collect_start_pos));
  const std::shared_ptr<srcml_node> & keyword_node_modified = out.nodes_modified().at(element_list_modified.at(start_modified).at(modified_collect_start_pos));
  bool is_keyword  = keyword_node_original->is_text() && !keyword_node_original->is_white_space();
  bool is_keywords = is_keyword
                     && keyword_node_modified->is_text() && !keyword_node_modified->is_white_space();
  bool is_same_keyword = is_keywords && srcdiff_compare::node_compare(keyword_node_original, keyword_node_modified) == 0;


  if((is_keyword && !is_keywords) || (is_keywords && !is_same_keyword)) {
    ++original_collect_start_pos;
  }

  // output deleted nodes
  out.diff_original_start->properties.push_back(diff_convert_type);
  out.output_node(out.diff_original_start, SES_DELETE, true);
  out.diff_original_start->properties.clear();

  for(int output_pos = 0; output_pos < original_collect_start_pos; ++output_pos) {
    out.output_node(out.nodes_original().at(element_list_original.at(start_original).at(output_pos)), SES_DELETE);
    ++out.last_output_original();
  }

  // output inserted nodes
  out.diff_modified_start->properties.push_back(diff_convert_type);
  out.output_node(out.diff_modified_start, SES_INSERT, true);
  out.diff_modified_start->properties.clear();

  if(is_keywords && !is_same_keyword){
    ++modified_collect_start_pos;
  }

  for(int output_pos = 0; output_pos < modified_collect_start_pos; ++output_pos) {
    out.output_node(out.nodes_modified().at(element_list_modified.at(start_modified).at(output_pos)), SES_INSERT);
    ++out.last_output_modified();
  }

  // collect subset of nodes
  element_list next_set_original
    = element_list(out.nodes_original(), element_list_original.at(start_original).at(original_collect_start_pos)
                      , element_list_original.at(start_original).back());

  element_list next_set_modified
    = element_list(out.nodes_modified(), element_list_modified.at(start_modified).at(modified_collect_start_pos)
                      , element_list_modified.at(start_modified).back());

  srcdiff_diff diff(out, next_set_original, next_set_modified);
  diff.output();

  output_whitespace();

  output_change(out.last_output_original(), element_list_modified.at(start_modified).back() + 1);

  out.output_node(out.diff_modified_end, SES_INSERT, true);
  if(out.output_state() == SES_INSERT) {
    out.output_node(out.diff_modified_end, SES_INSERT);
  }

  output_change(element_list_original.at(start_original).back() + 1, out.last_output_modified());

  out.output_node(out.diff_original_end, SES_DELETE, true);
  if(out.output_state() == SES_DELETE) {
      out.output_node(out.diff_original_end, SES_DELETE);
  }

}

void srcdiff_single::output() {

  const std::shared_ptr<srcml_node> & start_node_original = out.nodes_original().at(element_list_original.at(start_original).front());
  const std::shared_ptr<srcml_node> & start_node_modified = out.nodes_modified().at(element_list_modified.at(start_modified).front());

  if(start_node_original->name == start_node_modified->name
    && start_node_original->ns.href == start_node_modified->ns.href) {
    output_recursive_same();
  } else {
    output_recursive_interchangeable();
  }

}
