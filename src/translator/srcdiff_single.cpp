#include <srcdiff_single.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_comment.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_compare.hpp>
#include <shortest_edit_script.h>

#include <cstring>
#include <map>
#include <list>

const std::string srcdiff_single::convert("convert");

srcdiff_single::srcdiff_single(const srcdiff_many & diff, unsigned int start_original, unsigned int start_modified) : srcdiff_many(diff), start_original(start_original), start_modified(start_modified) {}

static std::list<srcml_node::srcml_attr> merge_properties(const std::list<srcml_node::srcml_attr> & properties_original, const std::list<srcml_node::srcml_attr> & properties_modified) {

  std::list<srcml_node::srcml_attr> attributes;

  auto citr_original = properties_original.begin();
  auto citr_modified = properties_modified.begin();

  while(citr_original != properties_original.end() && citr_modified != properties_modified.end()) {

    if(citr_original->name == citr_modified->name) {

      if(*citr_original->value == citr_modified->value) 
        attributes.emplace_back(citr_original->name, *citr_original->value);
      else
        attributes.emplace_back(citr_original->name, *citr_original->value + std::string("|") +*citr_modified->value);

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

  out.output_node(out.diff_common_start, SESCOMMON);

  std::shared_ptr<srcml_node> merged_node;

  if(srcdiff_compare::node_compare(out.get_nodes_original().at(node_sets_original.at(start_original).at(0)), out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))) == 0) {

    out.output_node(out.get_nodes_original().at(node_sets_original.at(start_original).at(0)), SESCOMMON);

  } else {

    merged_node = std::make_shared<srcml_node>(*out.get_nodes_original().at(node_sets_original.at(start_original).at(0)));

    merged_node->properties = merge_properties(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->properties,
                                              out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0))->properties);


    out.output_node(merged_node, SESCOMMON);


  }

  ++out.last_output_original();
  ++out.last_output_modified();

  // compare subset of nodes
  if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "comment") {

    // collect subset of nodes
    node_sets next_set_original
      = node_sets(out.get_nodes_original(), node_sets_original.at(start_original).at(1)
                        , node_sets_original.at(start_original).at(node_sets_original.at(start_original).size() - 1));

    node_sets next_set_modified
      = node_sets(out.get_nodes_modified(), node_sets_modified.at(start_modified).at(1)
                        , node_sets_modified.at(start_modified).at(node_sets_modified.at(start_modified).size() - 1));

    srcdiff_comment diff(out, next_set_original, next_set_modified);
    diff.output();

  } else {

      // collect subset of nodes
      node_sets next_set_original
        = node_sets(out.get_nodes_original(), node_sets_original.at(start_original).at(1)
                          , node_sets_original.at(start_original).back());

      node_sets next_set_modified
        = node_sets(out.get_nodes_modified(), node_sets_modified.at(start_modified).at(1)
                          , node_sets_modified.at(start_modified).back());

      srcdiff_diff diff(out, next_set_original, next_set_modified);
      diff.output();

  }

  output_common(node_sets_original.at(start_original).back() + 1, node_sets_modified.at(start_modified).back() + 1);

  out.output_node(out.diff_common_end, SESCOMMON);

  whitespace.output_statement();

}

void srcdiff_single::output_recursive_interchangeable() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  out.diff_type->value = convert;
  out.diff_original_start->properties.push_back(*out.diff_type.get());
  out.diff_modified_start->properties.push_back(*out.diff_type.get());

  out.output_node(out.diff_original_start, SESDELETE, true);
  out.diff_original_start->properties.clear();

  out.output_node(out.get_nodes_original().at(node_sets_original.at(start_original).at(0)), SESDELETE);

  int original_collect_start_pos = 1;

  // if do this then ; no longer matches
  // if(out.get_nodes_original().at(node_sets_original.at(start_original).at(0))->name == "decl_stmt") {

  //   while(out.get_nodes_original().at(node_sets_original.at(start_original).at(original_collect_start_pos))->name != "init") {

  //     ++original_collect_start_pos;

  //   }

  //   output_change(original_collect_start_pos + 1, out.last_output_modified());
  //   output_whitespace();

  // }

  const std::shared_ptr<srcml_node> & keyword_node_original = out.get_nodes_original().at(node_sets_original.at(start_original).at(original_collect_start_pos));
  const std::shared_ptr<srcml_node> & keyword_node_modified = out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(1));
  bool is_keyword = keyword_node_original->is_text() && !keyword_node_original->is_white_space();
  bool is_keywords = is_keyword
                     && keyword_node_modified->is_text() && !keyword_node_modified->is_white_space();
  bool is_same_keyword = is_keywords && srcdiff_compare::node_compare(keyword_node_original, keyword_node_modified) == 0;


  if((is_keyword && !is_keywords) || (is_keywords && !is_same_keyword)) {

    out.output_node(out.get_nodes_original().at(node_sets_original.at(start_original).at(1)), SESDELETE);
    ++out.last_output_original();
    ++original_collect_start_pos;

  }

  ++out.last_output_original();

  out.output_node(out.diff_modified_start, SESINSERT, true);
  out.diff_modified_start->properties.clear();

  out.output_node(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(0)), SESINSERT);

  int modified_collect_start_pos = 1;
  if(is_keywords && !is_same_keyword){

    out.output_node(out.get_nodes_modified().at(node_sets_modified.at(start_modified).at(1)), SESINSERT);
    ++out.last_output_modified();
    ++modified_collect_start_pos;

  }

  ++out.last_output_modified();

  // collect subset of nodes
  node_sets next_set_original
    = node_sets(out.get_nodes_original(), node_sets_original.at(start_original).at(original_collect_start_pos)
                      , node_sets_original.at(start_original).back());

  node_sets next_set_modified
    = node_sets(out.get_nodes_modified(), node_sets_modified.at(start_modified).at(modified_collect_start_pos)
                      , node_sets_modified.at(start_modified).back());

  srcdiff_diff diff(out, next_set_original, next_set_modified);
  diff.output();

  output_whitespace();

  output_change(out.last_output_original(), node_sets_modified.at(start_modified).back() + 1);

  out.output_node(out.diff_modified_end, SESINSERT, true);

  output_change(node_sets_original.at(start_original).back() + 1, out.last_output_modified());

  out.output_node(out.diff_original_end, SESDELETE, true);

  whitespace.output_statement();

}

void srcdiff_single::output() {

  const std::shared_ptr<srcml_node> & start_node_original = out.get_nodes_original().at(node_sets_original.at(start_original).front());
  const std::shared_ptr<srcml_node> & start_node_modified = out.get_nodes_modified().at(node_sets_modified.at(start_modified).front());

  if(start_node_original->name == start_node_modified->name
    && (bool(start_node_original->ns) == bool(start_node_modified->ns) && (!start_node_original->ns
      || (start_node_original->ns->prefix == start_node_modified->ns->prefix 
        && (!start_node_original->ns->prefix || *start_node_original->ns->prefix == *start_node_modified->ns->prefix)))))
    output_recursive_same();
  else
    output_recursive_interchangeable();

}
