#include <srcdiff_single.hpp>
#include <srcdiff_change.hpp>
#include <srcdiff_comment.hpp>

#include <shortest_edit_script.h>
#include <srcDiffCommon.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffWhiteSpace.hpp>
#include <srcDiffUtility.hpp>

#include <string.h>
#include <map>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

extern xAttr diff_type;

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
  whitespace.output_white_space_all();

  out.output_node(&diff_common_start, SESCOMMON);

  xNodePtr merged_node = 0;

  if(node_compare(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))) == 0) {

    out.output_node(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), SESCOMMON);

  } else {

    merged_node = copyXNode(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)));
    freeXAttr(merged_node->properties);

    merged_node->properties = merge_properties(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->properties,
                                              rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->properties);


    out.output_node(merged_node, SESCOMMON);


  }

  ++rbuf_old.last_output;
  ++rbuf_new.last_output;

  // compare subset of nodes
  if(strcmp((const char *)rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    node_sets next_set_old
      = node_sets(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    node_sets next_set_new
      = node_sets(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    srcdiff_comment diff(out, rbuf_old, rbuf_new, wstate, &next_set_old, &next_set_new);
    diff.output();

  } else {

      // collect subset of nodes
      node_sets next_set_old
        = node_sets(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                          , node_sets_old->at(start_old)->back());

      node_sets next_set_new
        = node_sets(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                          , node_sets_new->at(start_new)->back());

      srcdiff_diff diff(out, rbuf_old, rbuf_new, wstate, &next_set_old, &next_set_new);
      diff.output();

  }

  output_common(node_sets_old->at(start_old)->back() + 1, node_sets_new->at(start_new)->back() + 1);

  out.output_node(&diff_common_end, SESCOMMON);

  whitespace.output_white_space_statement();

  if(merged_node) freeXNode(merged_node);

}

void srcdiff_single::output_recursive_interchangeable() {

  srcdiff_whitespace whitespace(out);
  whitespace.output_white_space_all();

  out.output_node(&diff_old_start, SESDELETE);

  out.output_node(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), SESDELETE);

  bool is_same_keyword = node_compare(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(1)),
                  rbuf_new.nodes.at(node_sets_new->at(start_new)->at(1))) == 0;

  int old_collect_start_pos = 1;
  if(!is_same_keyword) {

    out.output_node(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(1)), SESDELETE);
    ++rbuf_old.last_output;
    old_collect_start_pos = 2;

  }

  ++rbuf_old.last_output;

  out.output_node(&diff_new_start, SESINSERT);

  out.output_node(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0)), SESINSERT);

  int new_collect_start_pos = 1;
  if(!is_same_keyword){

    out.output_node(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(1)), SESINSERT);
    ++rbuf_new.last_output;
    new_collect_start_pos = 2;

  }

  ++rbuf_new.last_output;

  // collect subset of nodes
  node_sets next_set_old
    = node_sets(rbuf_old.nodes, node_sets_old->at(start_old)->at(old_collect_start_pos)
                      , node_sets_old->at(start_old)->back());

  node_sets next_set_new
    = node_sets(rbuf_new.nodes, node_sets_new->at(start_new)->at(new_collect_start_pos)
                      , node_sets_new->at(start_new)->back());

  srcdiff_diff diff(out, rbuf_old, rbuf_new, wstate, &next_set_old, &next_set_new);
  diff.output();

  output_change(rbuf_old.last_output, node_sets_new->at(start_new)->back() + 1);

  out.output_node(&diff_new_end, SESINSERT);

  output_change(node_sets_old->at(start_old)->back() + 1, rbuf_new.last_output);

  out.output_node(&diff_old_end, SESDELETE);

  whitespace.output_white_space_statement();


}

void srcdiff_single::output() {

    xNodePtr start_node_old = rbuf_old.nodes.at(node_sets_old->at(start_old)->front());
    xNodePtr start_node_new = rbuf_new.nodes.at(node_sets_new->at(start_new)->front());

  if(strcmp((const char *)start_node_old->name, (const char *)start_node_new->name) == 0
    && (start_node_old->ns == start_node_new->ns 
      || (start_node_old->ns && start_node_old->ns && (start_node_old->ns->prefix == start_node_new->ns->prefix 
        || (start_node_old->ns->prefix && start_node_new->ns->prefix && strcmp((const char *)start_node_old->ns->prefix, (const char *)start_node_new->ns->prefix) == 0)))))
    output_recursive_same();
  else
    output_recursive_interchangeable();

}
