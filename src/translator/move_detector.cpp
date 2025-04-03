// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file move_detector.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <move_detector.hpp>
#include <measurer.hpp>
#include <whitespace_stream.hpp>

#include <vector>
#include <map>
#include <list>
#include <unordered_set>

namespace srcdiff {

int move_id = 0;
const std::string move("move");

typedef std::tuple<int, int> move_info;
typedef std::vector<move_info> move_infos;

move_detector::move_detector(const output_stream& out, std::size_t& position, enum operation operation)
  : output_stream(out), position(position), operation(operation) {}

bool move_detector::is_move(std::shared_ptr<const construct> set) {

  return set->term(0)->get_move();

}

/** loop O(CD^2) */
void move_detector::mark_moves(const construct::construct_list_view original,
                               const construct::construct_list_view modified,
                               edit_t* edit_script) {

  std::map<std::string, move_infos > constructs;

  typedef std::unordered_multiset<std::shared_ptr<const construct>>::iterator lookup_iterator;
  std::unordered_multiset<std::shared_ptr<const construct>> node_set_lookup_table;

  std::vector<std::shared_ptr<const construct>> delete_sets;

  for(edit_t * edits = edit_script; edits != nullptr; edits = edits->next) {

    switch(edits->operation) {

      case SES_COMMON:
        break;

      case SES_CHANGE:

        for(std::size_t i = 0; i < edits->length; ++i) {

          if(original[edits->offset_sequence_one + i]->term(0)->is_text()) {
            continue;
          }
          delete_sets.push_back(original[edits->offset_sequence_one + i]);

        }

        for(std::size_t i = 0; i < edits->length_two; ++i) {

          if(modified[edits->offset_sequence_two + i]->term(0)->is_text()) {
            continue;
          }
          node_set_lookup_table.insert(modified[edits->offset_sequence_two + i]);

        }

        break;

      case SES_INSERT:

        for(std::size_t i = 0; i < edits->length; ++i) {

          if(modified[edits->offset_sequence_two + i]->term(0)->is_text()) {
            continue;
          }
          node_set_lookup_table.insert(modified[edits->offset_sequence_two + i]);

        }

        break;

      case SES_DELETE: 

        for(std::size_t i = 0; i < edits->length; ++i) {

          if(original[edits->offset_sequence_one + i]->term(0)->is_text()) {
            continue;
          }
          delete_sets.push_back(original[edits->offset_sequence_one + i]);

        }

        break;

      }

  }

  for(std::shared_ptr<const construct> set : delete_sets) {

    std::pair<lookup_iterator, lookup_iterator> range = node_set_lookup_table.equal_range(set);

    for(lookup_iterator pos = std::get<0>(range); pos != std::get<1>(range); ++pos) {

      if(is_move(*pos)) {
        continue;
      }

      if(!(*set == **pos)) {
        continue;
      }

      ++move_id;
      std::shared_ptr<srcML::node> start_node_one = std::make_shared<srcML::node>(*set->term(0));
      start_node_one->set_move(move_id);

      std::shared_ptr<srcML::node> start_node_two = std::make_shared<srcML::node>(*(*pos)->term(0));
      start_node_two->set_move(move_id);

      // breaks const
      // Not sure if I can use terms here. Also, probably should fix the break const thing
      ((srcml_nodes &)set->nodes()).at(set->start_position()) = start_node_one;
      ((srcml_nodes &)(*pos)->nodes()).at((*pos)->start_position()) = start_node_two;

      if(!start_node_one->is_empty()) {

        std::shared_ptr<srcML::node> end_node_one = std::make_shared<srcML::node>(*set->last_term());
        end_node_one->set_move(move_id);

        std::shared_ptr<srcML::node> end_node_two = std::make_shared<srcML::node>(*(*pos)->last_term());
        end_node_two->set_move(move_id);

        ((srcml_nodes &)set->nodes()).at(set->end_position()) = end_node_one;
        ((srcml_nodes &)(*pos)->nodes()).at((*pos)->end_position()) = end_node_two;

      }

    }

  }

}

void move_detector::output() {

  // unused
  //static int attribute_id = 0;

  // store current diff if is any
  std::shared_ptr<reader_state> rbuf = rbuf_original;
  std::shared_ptr<srcML::node> start_node = diff_original_start;
  std::shared_ptr<srcML::node> end_node = diff_original_end;

  if(operation == INSERT) {

    rbuf = rbuf_modified;
    start_node = diff_modified_start;
    end_node = diff_modified_end;

  }

  int id = rbuf->nodes.at(position)->get_move();

  start_node->emplace_attribute(std::to_string(id), srcML::attribute(move, srcML::name_space::DIFF_NAMESPACE, std::to_string(id)));

  output_node(start_node, operation, true);

  output_node(rbuf->nodes.at(position), operation);

  if(!rbuf->nodes.at(position)->is_empty()) {

    ++position;

    for(; rbuf->nodes.at(position)->get_move() != id ; ++position) {

      if(rbuf->nodes.at(position)->is_whitespace()) {

        rbuf->last_output = position;
        whitespace_stream whitespace(*this);
        whitespace.output_all(operation);
        position = rbuf->last_output - 1;

        continue;

      }

      output_node(rbuf->nodes.at(position), operation);
    }

    output_node(rbuf->nodes.at(position), operation);

  }

  output_node(end_node, operation, true);

  start_node->clear_attributes();

}

}
