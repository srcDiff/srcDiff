#include <srcdiff_move.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_whitespace.hpp>

#include <vector>
#include <map>
#include <list>

int move_id = 0;
const std::string move("move");

typedef std::tuple<int, int, const edit *, int> move_info;
typedef std::vector<move_info> move_infos;

srcdiff_move::srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation)
  : srcdiff_output(out), position(position), operation(operation) {}


void add_construct(std::map<std::string, move_infos > & constructs,
                   const node_sets & sets,
                   int offset,
                   int operation,
                   const edit * anedit,
                   int edit_pos) {

  std::string tag = sets.nodes().at(sets.at(offset).at(0))->name;

  if(constructs.find(tag) == constructs.end()) {

    constructs[tag] = move_infos();

  }

  constructs[tag].push_back(move_info(offset, operation, anedit, edit_pos));

}

bool srcdiff_move::is_move(const node_set & set) {

  return set.nodes().at(set.at(0))->move;

}

void srcdiff_move::mark_moves(srcml_nodes & nodes_original,
                              const node_sets & node_sets_original,
                              srcml_nodes & nodes_modified,
                              const node_sets & node_sets_modified,
                              edit * edit_script) {

  std::map<std::string, move_infos > constructs;

  for(edit * edits = edit_script; edits; edits = edits->next) {

    switch(edits->operation) {

    case SES_INSERT :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, node_sets_modified, edits->offset_sequence_two + i, SES_INSERT, edits, i);

      }

      break;

    case SES_DELETE :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, node_sets_original, edits->offset_sequence_one + i, SES_DELETE, edits, i);

      }

      break;

    }

  }

  for(std::map<std::string, move_infos>::const_iterator construct = constructs.begin(); construct != constructs.end(); ++construct) {

    if(construct->first == "text")
      continue;

    move_infos elements = construct->second;

    for(unsigned int i = 0; i < elements.size(); ++i) {

      srcml_nodes * nodes_one = &nodes_original;
      srcml_nodes * nodes_two = &nodes_modified;

      const node_sets * node_sets_one = &node_sets_original;
      const node_sets * node_sets_two = &node_sets_modified;

      if(std::get<1>(elements.at(i)) == SES_INSERT) {

       nodes_one = &nodes_modified;
       nodes_two = &nodes_original;

        node_sets_one = &node_sets_modified;
        node_sets_two = &node_sets_original;

      }

      if(nodes_one->at(node_sets_one->at(std::get<0>(elements.at(i))).at(0))->move)
        continue;

      for(unsigned int j = i + 1; j < elements.size(); ++j) {

        if(std::get<1>(elements.at(i)) == std::get<1>(elements.at(j)))
          continue;

        diff_nodes diff_set = { *nodes_one, *nodes_two };

        if(srcdiff_compare::node_set_syntax_compare(&node_sets_one->at(std::get<0>(elements.at(i)))
                                   , &node_sets_two->at(std::get<0>(elements.at(j))), &diff_set) != 0)
          continue;
        /*
          if(compute_difference(nodes_one, node_sets_one->at(std::get<0>(elements.at(i)))
          , nodes_two, node_sets_two->at(std::get<0>(elements.at(j)))) != 0)
          continue;
        */

      	if(is_move(node_sets_one->at(std::get<0>(elements.at(i)))) || is_move(node_sets_two->at(std::get<0>(elements.at(j)))))
		     continue;

        if(std::get<1>(elements.at(i)) == SES_DELETE) {

          if(is_change(std::get<2>(elements.at(i)))
            && std::get<2>(elements.at(i))->next == std::get<2>(elements.at(j))
            && std::abs(std::get<3>(elements.at(i)) - std::get<3>(elements.at(j))) <= 1)
            continue;

        } else {

          if(is_change(std::get<2>(elements.at(j)))
            && std::get<2>(elements.at(j))->next == std::get<2>(elements.at(i))
            && std::abs(std::get<3>(elements.at(i)) - std::get<3>(elements.at(j))) <= 1)
            continue;

        }

        ++move_id;
        std::shared_ptr<srcml_node> start_node_one = std::make_shared<srcml_node>(*nodes_one->at(node_sets_one->at(std::get<0>(elements.at(i))).at(0)));
        start_node_one->move = move_id;

        std::shared_ptr<srcml_node> start_node_two = std::make_shared<srcml_node>(*nodes_two->at(node_sets_two->at(std::get<0>(elements.at(j))).at(0)));
        start_node_two->move = move_id;

        nodes_one->at(node_sets_one->at(std::get<0>(elements.at(i))).at(0)) = start_node_one;
        nodes_two->at(node_sets_two->at(std::get<0>(elements.at(j))).at(0)) = start_node_two;

        if(!start_node_one->is_empty) {

          std::shared_ptr<srcml_node> end_node_one = std::make_shared<srcml_node>(*nodes_one->at(node_sets_one->at(std::get<0>(elements.at(i))).back()));
          end_node_one->move = move_id;

          std::shared_ptr<srcml_node> end_node_two = std::make_shared<srcml_node>(*nodes_two->at(node_sets_two->at(std::get<0>(elements.at(j))).back()));
          end_node_two->move = move_id;

          nodes_one->at(node_sets_one->at(std::get<0>(elements.at(i))).back()) = end_node_one;
          nodes_two->at(node_sets_two->at(std::get<0>(elements.at(j))).back()) = end_node_two;

        }

        break;

      }

    }

  }

}

void srcdiff_move::output() {

  // store current diff if is any
  std::shared_ptr<reader_state> rbuf = rbuf_original;
  std::shared_ptr<srcml_node> start_node = diff_original_start;
  std::shared_ptr<srcml_node> end_node = diff_original_end;

  if(operation == SES_INSERT) {

    rbuf = rbuf_modified;
    start_node = diff_modified_start;
    end_node = diff_modified_end;

  }

  int id = rbuf->nodes.at(position)->move;

  start_node->properties.emplace_back(move, std::to_string(id));

  output_node(start_node, operation, true);

  output_node(rbuf->nodes.at(position), operation);

  if(!rbuf->nodes.at(position)->is_empty) {

    ++position;

    for(; rbuf->nodes.at(position)->move != id; ++position) {

      if(rbuf->nodes.at(position)->is_white_space()) {

        rbuf->last_output = position;
        srcdiff_whitespace whitespace(*this);
        whitespace.output_all(operation);
        position = rbuf->last_output - 1;
        continue;

      }

      output_node(rbuf->nodes.at(position), operation);

    }

    output_node(rbuf->nodes.at(position), operation);

  }

  output_node(end_node, operation, true);

  start_node->properties.clear();

}
