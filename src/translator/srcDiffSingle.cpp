#include <srcDiffSingle.hpp>

#include <shortest_edit_script.h>
#include <srcDiffCommentDiff.hpp>
#include <srcDiffCommon.hpp>
#include <srcDiffChange.hpp>
#include <srcDiffDiff.hpp>
#include <srcDiffNested.hpp>
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

void output_recursive_same(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

  output_white_space_all(rbuf_old, rbuf_new, wstate);
  //markup_common(rbuf_old, node_sets_old->at(start_old)->at(0), rbuf_new, node_sets_new->at(start_new)->at(0), wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_start, SESCOMMON, wstate);

  xNodePtr merged_node = 0;

  if(node_compare(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))) == 0) {

    output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), SESCOMMON, wstate);

  } else {

    std::vector<std::string> attribute_names;
    std::vector<std::string> attribute_values;
    for(xAttr * attr = rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->properties; attr; attr = attr->next) {

      const char * attribute = find_attribute(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0)), attr->name);
      attribute_names.push_back(attr->name);
      if(attribute == 0) {

        attribute_values.push_back(std::string(attr->value) + std::string("|"));


      } else {

        attribute_values.push_back(std::string(attr->name) + std::string("|") + std::string(attribute));


      }

    }

    for(xAttr * attr = rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->properties; attr; attr = attr->next) {

      const char * attribute = find_attribute(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0)), attr->name);
      if(attribute == 0) {

        attribute_names.push_back(attr->name);
        attribute_values.push_back(std::string("|") + std::string(attr->value));


      }

    }

    merged_node = copyXNode(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)));

    xAttrPtr save_properties = merged_node->properties;
    merged_node->properties = 0;
    xAttrPtr last_attr = 0;

    for(std::vector<std::string>::size_type pos = 0; pos < attribute_names.size(); ++pos) {

      xAttrPtr attr = new xAttr;
      attr->name = attribute_names[pos].c_str(), attr->value = attribute_values[pos].c_str(), attr->next = 0;

      if(last_attr == 0) merged_node->properties = attr;
      else last_attr->next = attr;

      last_attr = attr;

    }

    output_node(rbuf_old, rbuf_new, merged_node, SESCOMMON, wstate);

    for(xAttrPtr attr = merged_node->properties; attr;) {

        xAttrPtr save_attr = attr;
        attr = attr->next;
        delete save_attr;

    }

    merged_node->properties = save_properties;

  }

  ++rbuf_old.last_output;
  ++rbuf_new.last_output;

  // compare subset of nodes
  if(strcmp((const char *)rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    NodeSets next_node_set_old
      = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    NodeSets next_node_set_new
      = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    free_node_sets(next_node_set_old);
    free_node_sets(next_node_set_new);

  }
  else {

      // collect subset of nodes
      NodeSets next_node_set_old
        = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                          , node_sets_old->at(start_old)->back());

      NodeSets next_node_set_new
        = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                          , node_sets_new->at(start_new)->back());

      output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

      free_node_sets(next_node_set_old);
      free_node_sets(next_node_set_new);

  }

  output_common(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_end, SESCOMMON, wstate);

  output_white_space_statement(rbuf_old, rbuf_new, wstate);

  if(merged_node) freeXNode(merged_node);

}

void output_recursive_interchangeable(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

  output_white_space_all(rbuf_old, rbuf_new, wstate);

  output_node(rbuf_old, rbuf_new, &diff_old_start, SESDELETE, wstate);

  output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), SESDELETE, wstate);

  bool is_same_keyword = node_compare(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(1)),
                  rbuf_new.nodes.at(node_sets_new->at(start_new)->at(1))) == 0;

  int old_collect_start_pos = 1;
  if(!is_same_keyword) {

    output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(node_sets_old->at(start_old)->at(1)), SESDELETE, wstate);
    ++rbuf_old.last_output;
    old_collect_start_pos = 2;

  }

  ++rbuf_old.last_output;

  output_node(rbuf_old, rbuf_new, &diff_new_start, SESINSERT, wstate);

  output_node(rbuf_old, rbuf_new, rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0)), SESINSERT, wstate);

  int new_collect_start_pos = 1;
  if(!is_same_keyword){

    output_node(rbuf_old, rbuf_new, rbuf_new.nodes.at(node_sets_new->at(start_new)->at(1)), SESINSERT, wstate);
    ++rbuf_new.last_output;
    new_collect_start_pos = 2;

  }

  ++rbuf_new.last_output;

  // collect subset of nodes
  NodeSets next_node_set_old
    = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(old_collect_start_pos)
                      , node_sets_old->at(start_old)->back());

  NodeSets next_node_set_new
    = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(new_collect_start_pos)
                      , node_sets_new->at(start_new)->back());

  output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

  free_node_sets(next_node_set_old);
  free_node_sets(next_node_set_new);

  output_change(rbuf_old, rbuf_old.last_output, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  output_node(rbuf_old, rbuf_new, &diff_new_end, SESINSERT, wstate);

  output_change(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, rbuf_new.last_output, wstate);

  output_node(rbuf_old, rbuf_new, &diff_old_end, SESDELETE, wstate);

  output_white_space_statement(rbuf_old, rbuf_new, wstate);


}

void output_recursive(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

    xNodePtr start_node_old = rbuf_old.nodes.at(node_sets_old->at(start_old)->front());
    xNodePtr start_node_new = rbuf_new.nodes.at(node_sets_new->at(start_new)->front());

  if(strcmp((const char *)start_node_old->name, (const char *)start_node_new->name) == 0
    && (start_node_old->ns == start_node_new->ns 
      || (start_node_old->ns && start_node_old->ns && (start_node_old->ns->prefix == start_node_new->ns->prefix 
        || (start_node_old->ns->prefix && start_node_new->ns->prefix && strcmp((const char *)start_node_old->ns->prefix, (const char *)start_node_new->ns->prefix) == 0)))))
    output_recursive_same(rbuf_old, node_sets_old, start_old, rbuf_new, node_sets_new, start_new, wstate);
  else
    output_recursive_interchangeable(rbuf_old, node_sets_old, start_old, rbuf_new, node_sets_new, start_new, wstate);

}
