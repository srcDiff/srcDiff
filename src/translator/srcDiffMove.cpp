#include <srcDiffMove.hpp>

#include <vector>
#include <map>

#include <xmlrw.hpp>
#include <srcDiffMeasure.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffUtility.hpp>

int move_id = 0;

extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

xAttr move_attribute = { 0, "move", 0 };

void add_construct(std::map<std::string, IntPairs > & constructs
                   , NodeSets & node_sets, std::vector<xNodePtr> & nodes
                   , int offset, int operation) {

  std::string tag = nodes.at(node_sets.at(offset)->at(0))->name;

  if(constructs.find(tag) == constructs.end()) {

    constructs[tag] = IntPairs();

  }

  constructs[tag].push_back(IntPair(offset, operation));

}

bool is_move(NodeSet * node_set, std::vector<xNodePtr> nodes) {

  return nodes.at(node_set->at(0))->move;

}

void mark_moves(reader_state & rbuf_old, NodeSets * node_sets_old
                , reader_state & rbuf_new, NodeSets * node_sets_new
                , edit * edit_script, writer_state & wstate) {

  std::map<std::string, IntPairs > constructs;

  for(edit * edits = edit_script; edits; edits = edits->next) {

    switch(edits->operation) {

    case SESINSERT :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, *node_sets_new, rbuf_new.nodes, edits->offset_sequence_two + i, SESINSERT);

      }

      break;

    case SESDELETE :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, *node_sets_old, rbuf_old.nodes, edits->offset_sequence_one + i, SESDELETE);

      }

      break;

    }

  }

  for(std::map<std::string, IntPairs>::const_iterator construct = constructs.begin(); construct != constructs.end(); ++construct) {

    if(construct->first == "text")
      continue;

    IntPairs elements = construct->second;

    for(unsigned int i = 0; i < elements.size(); ++i) {

      reader_state * rbuf_one = &rbuf_old;
      reader_state * rbuf_two = &rbuf_new;

      NodeSets * node_sets_one = node_sets_old;
      NodeSets * node_sets_two = node_sets_new;

      if(elements.at(i).second == SESINSERT) {

        rbuf_one = &rbuf_new;
        rbuf_two = &rbuf_old;

        node_sets_one = node_sets_new;
        node_sets_two = node_sets_old;

      }

      if(rbuf_one->nodes.at(node_sets_one->at(elements.at(i).first)->at(0))->move)
        continue;

      for(unsigned int j = i + 1; j < elements.size(); ++j) {

        if(elements.at(i).second == elements.at(j).second)
          continue;

        diff_nodes diff_set = { rbuf_one->nodes, rbuf_two->nodes };

        if(node_set_syntax_compare(node_sets_one->at(elements.at(i).first)
                                   , node_sets_two->at(elements.at(j).first), &diff_set) != 0)
          continue;
        /*
          if(compute_difference(rbuf_one->nodes, node_sets_one->at(elements.at(i).first)
          , rbuf_two->nodes, node_sets_two->at(elements.at(j).first)) != 0)
          continue;
        */

	if(is_move(node_sets_one->at(elements.at(i).first), rbuf_one->nodes) || is_move(node_sets_two->at(elements.at(j).first), rbuf_two->nodes))
		   continue;

        ++move_id;
        xNode * start_node_one = copyXNode(rbuf_one->nodes.at(node_sets_one->at(elements.at(i).first)->at(0)));
        start_node_one->move = move_id;

        xNode * start_node_two = copyXNode(rbuf_two->nodes.at(node_sets_two->at(elements.at(j).first)->at(0)));
        start_node_two->move = move_id;

        rbuf_one->nodes.at(node_sets_one->at(elements.at(i).first)->at(0)) = start_node_one;
        rbuf_two->nodes.at(node_sets_two->at(elements.at(j).first)->at(0)) = start_node_two;

        if(!start_node_one->is_empty) {

          xNode * end_node_one = copyXNode(rbuf_one->nodes.at(node_sets_one->at(elements.at(i).first)->back()));
          end_node_one->move = move_id;

          xNode * end_node_two = copyXNode(rbuf_two->nodes.at(node_sets_two->at(elements.at(j).first)->back()));
          end_node_two->move = move_id;

          rbuf_one->nodes.at(node_sets_one->at(elements.at(i).first)->back()) = end_node_one;
          rbuf_two->nodes.at(node_sets_two->at(elements.at(j).first)->back()) = end_node_two;

        }

        break;

      }

    }

  }

}

void output_move(reader_state & rbuf_old, reader_state & rbuf_new, unsigned int & position, int operation, writer_state & wstate) {

  // store current diff if is any
  reader_state * rbuf = &rbuf_old;
  xNodePtr start_node = &diff_old_start;
  xNodePtr end_node = &diff_old_end;

  if(operation == SESINSERT) {

    rbuf = &rbuf_new;
    start_node = &diff_new_start;
    end_node = &diff_new_end;

  }

  int id = rbuf->nodes.at(position)->move;

  if(!id)
    return;

  int temp_count = id;
  int length;
  for(length = 0; temp_count > 0; temp_count /= 10, ++length)
    ;

  ++length;

  char * buffer = (char *)malloc(sizeof(char) * length);

  snprintf(buffer, length, "%d", id);

  move_attribute.value = buffer;

  xAttr * save_attributes = start_node->properties;
  start_node->properties = &move_attribute;

  output_node(rbuf_old, rbuf_new, start_node, SESMOVE, wstate);

  output_node(rbuf_old, rbuf_new, rbuf->nodes.at(position), SESMOVE, wstate);

  if(!rbuf->nodes.at(position)->is_empty) {

    ++position;

    for(; rbuf->nodes.at(position)->move != id; ++position) {

      output_node(rbuf_old, rbuf_new, rbuf->nodes.at(position), SESMOVE, wstate);

    }

    output_node(rbuf_old, rbuf_new, rbuf->nodes.at(position), SESMOVE, wstate);

  }

  output_node(rbuf_old, rbuf_new, end_node, SESMOVE, wstate);

  start_node->properties = save_attributes;
  free(buffer);

  // output saved diff if is any

}
