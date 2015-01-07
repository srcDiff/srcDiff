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

srcdiff_single::srcdiff_single(const srcdiff_many & diff, unsigned int start_old, unsigned int start_new) : srcdiff_many(diff), start_old(start_old), start_new(start_new) {}

static std::list<srcml_node::srcml_attr> merge_properties(const std::list<srcml_node::srcml_attr> & properties_old, const std::list<srcml_node::srcml_attr> & properties_new) {

  std::list<srcml_node::srcml_attr> attributes;

  auto citr_old = properties_old.begin();
  auto citr_new = properties_new.begin();

  while(citr_old != properties_old.end() && citr_new != properties_new.end()) {

    if(citr_old->name == citr_new->name) {

      if(*citr_old->value == citr_new->value) 
        attributes.emplace_back(citr_old->name, *citr_old->value);
      else
        attributes.emplace_back(citr_old->name, *citr_old->value + std::string("|") +*citr_new->value);

      ++citr_old;
      ++citr_new;

    } else {

      ++citr_new;

      bool is_end = citr_new == properties_new.end();
      std::string name;
      if(!is_end) name = citr_new->name;

      --citr_new;

      if(!is_end && citr_old->name == name) {

        attributes.emplace_back(citr_new->name, std::string("|") + *citr_new->value);

        ++citr_new;

      } else {

        attributes.emplace_back(citr_old->name, *citr_old->value + std::string("|"));

        ++citr_old;

      }

    }

  }

  while(citr_old != properties_old.end()) {

      attributes.emplace_back(citr_old->name, *citr_old->value + std::string("|"));

      ++citr_old;

  }

  while(citr_new != properties_new.end()) {

      attributes.emplace_back(citr_new->name, std::string("|") + *citr_new->value);

      ++citr_new;

  }

  return attributes;

}

void srcdiff_single::output_recursive_same() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  out.output_node(out.diff_common_start, SESCOMMON);

  std::shared_ptr<srcml_node> merged_node;

  if(srcdiff_compare::node_compare(out.get_nodes_old().at(node_sets_old.at(start_old).at(0)), out.get_nodes_new().at(node_sets_new.at(start_new).at(0))) == 0) {

    out.output_node(out.get_nodes_old().at(node_sets_old.at(start_old).at(0)), SESCOMMON);

  } else {

    merged_node = std::make_shared<srcml_node>(*out.get_nodes_old().at(node_sets_old.at(start_old).at(0)));

    merged_node->properties = merge_properties(out.get_nodes_old().at(node_sets_old.at(start_old).at(0))->properties,
                                              out.get_nodes_new().at(node_sets_new.at(start_new).at(0))->properties);


    out.output_node(merged_node, SESCOMMON);


  }

  ++out.last_output_old();
  ++out.last_output_new();

  // compare subset of nodes
  if(out.get_nodes_old().at(node_sets_old.at(start_old).at(0))->name == "comment") {

    // collect subset of nodes
    node_sets next_set_old
      = node_sets(out.get_nodes_old(), node_sets_old.at(start_old).at(1)
                        , node_sets_old.at(start_old).at(node_sets_old.at(start_old).size() - 1));

    node_sets next_set_new
      = node_sets(out.get_nodes_new(), node_sets_new.at(start_new).at(1)
                        , node_sets_new.at(start_new).at(node_sets_new.at(start_new).size() - 1));

    srcdiff_comment diff(out, next_set_old, next_set_new);
    diff.output();

  } else {

      // collect subset of nodes
      node_sets next_set_old
        = node_sets(out.get_nodes_old(), node_sets_old.at(start_old).at(1)
                          , node_sets_old.at(start_old).back());

      node_sets next_set_new
        = node_sets(out.get_nodes_new(), node_sets_new.at(start_new).at(1)
                          , node_sets_new.at(start_new).back());

      srcdiff_diff diff(out, next_set_old, next_set_new);
      diff.output();

  }

  output_common(node_sets_old.at(start_old).back() + 1, node_sets_new.at(start_new).back() + 1);

  out.output_node(out.diff_common_end, SESCOMMON);

  whitespace.output_statement();

}

void srcdiff_single::output_recursive_interchangeable() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  out.output_node(out.diff_old_start, SESDELETE);

  out.output_node(out.get_nodes_old().at(node_sets_old.at(start_old).at(0)), SESDELETE);

  bool is_same_keyword = srcdiff_compare::node_compare(out.get_nodes_old().at(node_sets_old.at(start_old).at(1)),
                  out.get_nodes_new().at(node_sets_new.at(start_new).at(1))) == 0;

  int old_collect_start_pos = 1;
  if(!is_same_keyword) {

    out.output_node(out.get_nodes_old().at(node_sets_old.at(start_old).at(1)), SESDELETE);
    ++out.last_output_old();
    old_collect_start_pos = 2;

  }

  ++out.last_output_old();

  out.output_node(out.diff_new_start, SESINSERT);

  out.output_node(out.get_nodes_new().at(node_sets_new.at(start_new).at(0)), SESINSERT);

  int new_collect_start_pos = 1;
  if(!is_same_keyword){

    out.output_node(out.get_nodes_new().at(node_sets_new.at(start_new).at(1)), SESINSERT);
    ++out.last_output_new();
    new_collect_start_pos = 2;

  }

  ++out.last_output_new();

  // collect subset of nodes
  node_sets next_set_old
    = node_sets(out.get_nodes_old(), node_sets_old.at(start_old).at(old_collect_start_pos)
                      , node_sets_old.at(start_old).back());

  node_sets next_set_new
    = node_sets(out.get_nodes_new(), node_sets_new.at(start_new).at(new_collect_start_pos)
                      , node_sets_new.at(start_new).back());

  srcdiff_diff diff(out, next_set_old, next_set_new);
  diff.output();

  output_change(out.last_output_old(), node_sets_new.at(start_new).back() + 1);

  out.output_node(out.diff_new_end, SESINSERT);

  output_change(node_sets_old.at(start_old).back() + 1, out.last_output_new());

  out.output_node(out.diff_old_end, SESDELETE);

  whitespace.output_statement();


}

void srcdiff_single::output() {

    const std::shared_ptr<srcml_node> & start_node_old = out.get_nodes_old().at(node_sets_old.at(start_old).front());
    const std::shared_ptr<srcml_node> & start_node_new = out.get_nodes_new().at(node_sets_new.at(start_new).front());

  if(start_node_old->name == start_node_new->name
    && (bool(start_node_old->ns) == bool(start_node_new->ns) && (!start_node_old->ns
      || (start_node_old->ns->prefix == start_node_new->ns->prefix 
        && (!start_node_old->ns->prefix || *start_node_old->ns->prefix == *start_node_new->ns->prefix)))))
    output_recursive_same();
  else
    output_recursive_interchangeable();

}
