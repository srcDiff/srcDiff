#include <srcdiff_move.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>

#include <vector>
#include <map>

int move_id = 0;

static srcml_node::srcml_attr move_attribute = { 0, std::string("move"), 0 };

typedef std::pair<int, int> IntPair;
typedef std::vector<IntPair> IntPairs;

srcdiff_move::srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation)
  : srcdiff_output(out), position(position), operation(operation) {}


void add_construct(std::map<std::string, IntPairs > & constructs
                   , const node_sets & sets, const std::vector<srcml_node *> & nodes
                   , int offset, int operation) {

  std::string tag = nodes.at(sets.at(offset).at(0))->name;

  if(constructs.find(tag) == constructs.end()) {

    constructs[tag] = IntPairs();

  }

  constructs[tag].push_back(IntPair(offset, operation));

}

bool srcdiff_move::is_move(const node_set & set, const std::vector<srcml_node *> & nodes) {

  return nodes.at(set.at(0))->move;

}

void srcdiff_move::mark_moves(std::vector<srcml_node *> & nodes_old, const node_sets & node_sets_old
                , std::vector<srcml_node *> & nodes_new, const node_sets & node_sets_new
                , edit * edit_script) {

  std::map<std::string, IntPairs > constructs;

  for(edit * edits = edit_script; edits; edits = edits->next) {

    switch(edits->operation) {

    case SESINSERT :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, node_sets_new, nodes_new, edits->offset_sequence_two + i, SESINSERT);

      }

      break;

    case SESDELETE :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, node_sets_old, nodes_old, edits->offset_sequence_one + i, SESDELETE);

      }

      break;

    }

  }

  for(std::map<std::string, IntPairs>::const_iterator construct = constructs.begin(); construct != constructs.end(); ++construct) {

    if(construct->first == "text")
      continue;

    IntPairs elements = construct->second;

    for(unsigned int i = 0; i < elements.size(); ++i) {

      std::vector<srcml_node *> * nodes_one = &nodes_old;
      std::vector<srcml_node *> * nodes_two = &nodes_new;

      const node_sets * node_sets_one = &node_sets_old;
      const node_sets * node_sets_two = &node_sets_new;

      if(elements.at(i).second == SESINSERT) {

       nodes_one = &nodes_new;
       nodes_two = &nodes_old;

        node_sets_one = &node_sets_new;
        node_sets_two = &node_sets_old;

      }

      if(nodes_one->at(node_sets_one->at(elements.at(i).first).at(0))->move)
        continue;

      for(unsigned int j = i + 1; j < elements.size(); ++j) {

        if(elements.at(i).second == elements.at(j).second)
          continue;

        diff_nodes diff_set = { *nodes_one, *nodes_two };

        if(srcdiff_compare::node_set_syntax_compare(&node_sets_one->at(elements.at(i).first)
                                   , &node_sets_two->at(elements.at(j).first), &diff_set) != 0)
          continue;
        /*
          if(compute_difference(nodes_one, node_sets_one->at(elements.at(i).first)
          , nodes_two, node_sets_two->at(elements.at(j).first)) != 0)
          continue;
        */

	if(is_move(node_sets_one->at(elements.at(i).first), *nodes_one) || is_move(node_sets_two->at(elements.at(j).first), *nodes_two))
		   continue;

        ++move_id;
        srcml_node * start_node_one = new srcml_node(*nodes_one->at(node_sets_one->at(elements.at(i).first).at(0)));
        start_node_one->move = move_id;

        srcml_node * start_node_two = new srcml_node(*nodes_two->at(node_sets_two->at(elements.at(j).first).at(0)));
        start_node_two->move = move_id;

        nodes_one->at(node_sets_one->at(elements.at(i).first).at(0)) = start_node_one;
        nodes_two->at(node_sets_two->at(elements.at(j).first).at(0)) = start_node_two;

        if(!start_node_one->is_empty) {

          srcml_node * end_node_one = new srcml_node(*nodes_one->at(node_sets_one->at(elements.at(i).first).back()));
          end_node_one->move = move_id;

          srcml_node * end_node_two = new srcml_node(*nodes_two->at(node_sets_two->at(elements.at(j).first).back()));
          end_node_two->move = move_id;

          nodes_one->at(node_sets_one->at(elements.at(i).first).back()) = end_node_one;
          nodes_two->at(node_sets_two->at(elements.at(j).first).back()) = end_node_two;

        }

        break;

      }

    }

  }

}

void srcdiff_move::output() {

  // store current diff if is any
  std::shared_ptr<reader_state> rbuf = rbuf_old;
  std::shared_ptr<srcml_node> start_node = diff_old_start;
  std::shared_ptr<srcml_node> end_node = diff_old_end;

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

  srcml_node::srcml_attr * save_attributes = start_node->properties;
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
