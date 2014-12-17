#include <srcdiff_move.hpp>

#include <vector>
#include <map>

#include <srcDiffMeasure.hpp>
#include <srcDiffUtility.hpp>

int move_id = 0;

static xAttr move_attribute = { 0, "move", 0 };

typedef std::pair<int, int> IntPair;
typedef std::vector<IntPair> IntPairs;

srcdiff_move::srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation)
  : srcdiff_output(out), position(position), operation(operation) {}


void add_construct(std::map<std::string, IntPairs > & constructs
                   , node_sets & sets, std::vector<xNodePtr> & nodes
                   , int offset, int operation) {

  std::string tag = nodes.at(sets.at(offset)->at(0))->name;

  if(constructs.find(tag) == constructs.end()) {

    constructs[tag] = IntPairs();

  }

  constructs[tag].push_back(IntPair(offset, operation));

}

bool srcdiff_move::is_move(node_set * set, std::vector<xNodePtr> nodes) {

  return nodes.at(set->at(0))->move;

}

void srcdiff_move::mark_moves(reader_state & rbuf_old, node_sets * node_sets_old
                , reader_state & rbuf_new, node_sets * node_sets_new
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

      node_sets * node_sets_one = node_sets_old;
      node_sets * node_sets_two = node_sets_new;

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

void srcdiff_move::output() {

  // store current diff if is any
  std::shared_ptr<reader_state> rbuf = rbuf_old;
  std::shared_ptr<xNode> start_node = diff_old_start;
  std::shared_ptr<xNode> end_node = diff_old_end;

  if(operation == SESINSERT) {

    rbuf = rbuf_new;
    start_node = diff_new_start;
    end_node = diff_new_end;

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

  output_node(start_node.get(), SESMOVE);

  output_node(rbuf->nodes.at(position), SESMOVE);

  if(!rbuf->nodes.at(position)->is_empty) {

    ++position;

    for(; rbuf->nodes.at(position)->move != id; ++position) {

      output_node(rbuf->nodes.at(position), SESMOVE);

    }

    output_node(rbuf->nodes.at(position), SESMOVE);

  }

  output_node(end_node.get(), SESMOVE);

  start_node->properties = save_attributes;
  free(buffer);

  // output saved diff if is any

}
