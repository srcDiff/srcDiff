#include <srcdiff_single.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_comment.hpp>
#include <srcdiff_common.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_compare.hpp>
#include <shortest_edit_script.h>

#include <cstring>
#include <map>

srcdiff_single::srcdiff_single(const srcdiff_many & diff, unsigned int start_old, unsigned int start_new) : srcdiff_many(diff), start_old(start_old), start_new(start_new) {}

static xAttrPtr merge_properties(xAttrPtr properties_old, xAttrPtr properties_new) {

  std::vector<std::string> attribute_names;
  std::vector<std::string> attribute_values;

  xAttrPtr oproperties = properties_old;
  xAttrPtr nproperties = properties_new;
  while(oproperties && nproperties) {

    if(strcmp(oproperties->name, nproperties->name) == 0) {

      attribute_names.push_back(oproperties->name);
      if(strcmp(oproperties->value, nproperties->value) == 0) 
        attribute_values.push_back(oproperties->value);
      else
        attribute_values.push_back(std::string(oproperties->value) + std::string("|") + std::string(nproperties->value));

      oproperties = oproperties->next;
      nproperties = nproperties->next;

    } else if(nproperties->next && strcmp(oproperties->name, nproperties->next->name) == 0) {

      attribute_names.push_back(nproperties->name);
      attribute_values.push_back(std::string("|") + std::string(nproperties->value));

      nproperties = nproperties->next;

    } else {

      attribute_names.push_back(oproperties->name);
      attribute_values.push_back(std::string(oproperties->value) + std::string("|"));

      oproperties = oproperties->next;

    }


  }

  while(oproperties) {

      attribute_names.push_back(oproperties->name);
      attribute_values.push_back(std::string(oproperties->value) + std::string("|"));

      oproperties = oproperties->next;

  }

  while(nproperties) {

      attribute_names.push_back(nproperties->name);
      attribute_values.push_back(std::string("|") + std::string(nproperties->value));

      nproperties = nproperties->next;

  }

  xAttrPtr first = 0;
  xAttrPtr last_attr = 0;

  for(std::vector<std::string>::size_type pos = 0; pos < attribute_names.size(); ++pos) {

    xAttrPtr attr = new xAttr;
    attr->name = strdup(attribute_names[pos].c_str()), attr->value = strdup(attribute_values[pos].c_str()), attr->next = 0;

    if(last_attr == 0) first = attr;
    else last_attr->next = attr;

    last_attr = attr;

  }

  return first;

}

void srcdiff_single::output_recursive_same() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  out.output_node(out.diff_common_start.get(), SESCOMMON);

  xNodePtr merged_node = 0;

  if(srcdiff_compare::node_compare(out.get_nodes_old().at(node_sets_old.at(start_old).at(0)), out.get_nodes_new().at(node_sets_new.at(start_new).at(0))) == 0) {

    out.output_node(out.get_nodes_old().at(node_sets_old.at(start_old).at(0)), SESCOMMON);

  } else {

    merged_node = copyXNode(out.get_nodes_old().at(node_sets_old.at(start_old).at(0)));
    freeXAttr(merged_node->properties);

    merged_node->properties = merge_properties(out.get_nodes_old().at(node_sets_old.at(start_old).at(0))->properties,
                                              out.get_nodes_new().at(node_sets_new.at(start_new).at(0))->properties);


    out.output_node(merged_node, SESCOMMON);


  }

  ++out.last_output_old();
  ++out.last_output_new();

  // compare subset of nodes
  if(strcmp((const char *)out.get_nodes_old().at(node_sets_old.at(start_old).at(0))->name, "comment") == 0) {

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

  out.output_node(out.diff_common_end.get(), SESCOMMON);

  whitespace.output_statement();

  if(merged_node) freeXNode(merged_node);

}

void srcdiff_single::output_recursive_interchangeable() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_all();

  out.output_node(out.diff_old_start.get(), SESDELETE);

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

  out.output_node(out.diff_new_start.get(), SESINSERT);

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

  out.output_node(out.diff_new_end.get(), SESINSERT);

  output_change(node_sets_old.at(start_old).back() + 1, out.last_output_new());

  out.output_node(out.diff_old_end.get(), SESDELETE);

  whitespace.output_statement();


}

void srcdiff_single::output() {

    xNodePtr start_node_old = out.get_nodes_old().at(node_sets_old.at(start_old).front());
    xNodePtr start_node_new = out.get_nodes_new().at(node_sets_new.at(start_new).front());

  if(strcmp((const char *)start_node_old->name, (const char *)start_node_new->name) == 0
    && (start_node_old->ns == start_node_new->ns 
      || (start_node_old->ns && start_node_old->ns && (start_node_old->ns->prefix == start_node_new->ns->prefix 
        || (start_node_old->ns->prefix && start_node_new->ns->prefix && strcmp((const char *)start_node_old->ns->prefix, (const char *)start_node_new->ns->prefix) == 0)))))
    output_recursive_same();
  else
    output_recursive_interchangeable();

}
