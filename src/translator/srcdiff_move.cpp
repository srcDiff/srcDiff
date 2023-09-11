#include <srcdiff_move.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_whitespace.hpp>

#include <vector>
#include <map>
#include <list>
#include <unordered_set>

int move_id = 0;
const std::string move("move");

typedef std::tuple<int, int> move_info;
typedef std::vector<move_info> move_infos;

srcdiff_move::srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation)
  : srcdiff_output(out), position(position), operation(operation) {}

bool srcdiff_move::is_move(const std::shared_ptr<construct> & set) {

  return set->term(0)->move;

}

/** loop O(CD^2) */
void srcdiff_move::mark_moves(const construct::construct_list & construct_list_original,
                              const construct::construct_list & construct_list_modified,
                              edit_t * edit_script) {

  std::map<std::string, move_infos > constructs;

  typedef std::unordered_multiset<std::shared_ptr<construct>>::iterator lookup_iterator;
  std::unordered_multiset<std::shared_ptr<construct>> node_set_lookup_table;

  std::vector<std::shared_ptr<construct>> delete_sets;

  for(edit_t * edits = edit_script; edits != nullptr; edits = edits->next) {

    switch(edits->operation) {

      case SES_COMMON:
        break;

      case SES_INSERT :

        for(int i = 0; i < edits->length; ++i) {

          if(construct_list_modified.at(edits->offset_sequence_two + i)->term(0)->is_text()) {
            continue;
          }
          node_set_lookup_table.insert(construct_list_modified.at(edits->offset_sequence_two + i));

        }

        break;

      case SES_DELETE :

        for(int i = 0; i < edits->length; ++i) {

          if(construct_list_original.at(edits->offset_sequence_one + i)->term(0)->is_text()) {
            continue;
          }
          delete_sets.push_back(construct_list_original.at(edits->offset_sequence_one + i));

        }

        break;

      }

  }

  for(const std::shared_ptr<construct> set : delete_sets) {

    std::pair<lookup_iterator, lookup_iterator> range = node_set_lookup_table.equal_range(set);

    for(lookup_iterator pos = std::get<0>(range); pos != std::get<1>(range); ++pos) {

      if(is_move(*pos)) {
        continue;
      }

      if(!(*set == **pos)) {
        continue;
      }

      ++move_id;
      std::shared_ptr<srcml_node> start_node_one = std::make_shared<srcml_node>(*set->term(0));
      start_node_one->move = move_id;

      std::shared_ptr<srcml_node> start_node_two = std::make_shared<srcml_node>(*(*pos)->term(0));
      start_node_two->move = move_id;

      // breaks const
      // Not sure if I can use terms here. Also, probably should fix the break const thing
      ((srcml_nodes &)set->nodes()).at(set->start_position()) = start_node_one;
      ((srcml_nodes &)(*pos)->nodes()).at((*pos)->start_position()) = start_node_two;

      if(!start_node_one->is_empty()) {

        std::shared_ptr<srcml_node> end_node_one = std::make_shared<srcml_node>(*set->last_term());
        end_node_one->move = move_id;

        std::shared_ptr<srcml_node> end_node_two = std::make_shared<srcml_node>(*(*pos)->last_term());
        end_node_two->move = move_id;

        ((srcml_nodes &)set->nodes()).at(set->end_position()) = end_node_one;
        ((srcml_nodes &)(*pos)->nodes()).at((*pos)->end_position()) = end_node_two;

      }

    }

  }

}

void srcdiff_move::output() {

  static int attribute_id = 0;

  // store current diff if is any
  std::shared_ptr<reader_state> rbuf = rbuf_original;
  std::shared_ptr<srcml_node> start_node = diff_original_start;
  std::shared_ptr<srcml_node> end_node = diff_original_end;

  if(operation == SES_INSERT) {

    rbuf = rbuf_modified;
    start_node = diff_modified_start;
    end_node = diff_modified_end;

  }

  int id = rbuf->nodes.at(position)->get_move();

  start_node->set_attributes({{std::to_string(id), move}});

  output_node(start_node, operation, true);

  output_node(rbuf->nodes.at(position), operation);

  if(!rbuf->nodes.at(position)->is_empty()) {

    ++position;

    for(; rbuf->nodes.at(position)->move != id ; ++position) {

      if(rbuf->nodes.at(position)->is_whitespace()) {

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

  start_node->attributes.clear();

}
