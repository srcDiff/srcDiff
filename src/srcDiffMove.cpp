#include "srcDiffMove.hpp"

#include <vector>
#include <map>

#include "xmlrw.hpp"
#include "shortest_edit_script.h"
#include "srcDiffMeasure.hpp"
#include "srcDiffOutput.hpp"

static int move_id;

void add_construct(std::map<std::string, std::vector<std::pair<int, int> > > & constructs
                   , std::vector<std::vector<int> *> & node_sets, std::vector<xNodePtr> & nodes
                   , int offset, int operation) {

  std::string tag = nodes.at(node_sets.at(offset)->at(0))->name;

  if(constructs.find(tag) == constructs.end()) {

    constructs[tag] = std::vector<std::pair<int, int> >();

  }

  constructs[tag].push_back(std::pair<int, int>(offset, operation));

}

void mark_moves(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                , edit * edit_script, writer_state & wstate) {

  std::map<std::string, std::vector<std::pair<int, int> > > constructs;

  for(edit * edits = edit_script; edits; edits = edits->next) {

    switch(edits->operation) {

    case SESINSERT :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, *node_sets_new, rbuf_new.nodes, edits->offset_sequence_two, SESINSERT);

      }

      break;

    case SESDELETE :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, *node_sets_old, rbuf_old.nodes, edits->offset_sequence_one, SESDELETE);

      }

      break;

    }

  }

  std::vector<std::pair<int, int> > functions = constructs["function"];

  for(unsigned int i = 0; i < functions.size(); ++i) {

    reader_state & rbuf_one = rbuf_old;
    reader_state & rbuf_two = rbuf_new;

    std::vector<std::vector<int> *> & node_sets_one = *node_sets_old;
    std::vector<std::vector<int> *> & node_sets_two = *node_sets_new;

    if(functions.at(i).second == SESINSERT) {

      rbuf_one = rbuf_new;
      rbuf_two = rbuf_old;

      node_sets_one = *node_sets_new;
      node_sets_two = *node_sets_old;

    }

    if(rbuf_one.nodes.at(node_sets_one.at(functions.at(i).first)->at(0))->move)
       continue;

    for(unsigned int j = i + 1; j < functions.size(); ++j) {

      if(functions.at(i).second == functions.at(j).second) 
       continue;

      if(compute_difference(rbuf_one.nodes, node_sets_one.at(functions.at(i).first), rbuf_two.nodes, node_sets_two.at(functions.at(j).first)) != 0)
        continue;

      ++move_id;

      rbuf_one.nodes.at(node_sets_one.at(functions.at(i).first)->at(0))->move = move_id;
      rbuf_two.nodes.at(node_sets_two.at(functions.at(j).first)->at(0))->move = move_id;

      rbuf_one.nodes.at(node_sets_one.at(functions.at(i).first)->back())->move = move_id;
      rbuf_two.nodes.at(node_sets_two.at(functions.at(j).first)->back())->move = move_id;

    }

  }

}

void output_move(reader_state & rbuf_old, reader_state & rbuf_new, unsigned int & position, int operation, writer_state & wstate) {

  // store current diff if is any

  reader_state rbuf = rbuf_old;

  if(operation == SESINSERT)
    rbuf = rbuf_new;

  int id = rbuf.nodes.at(position)->move;

    if(!id)
      return;

    output_node(rbuf_old, rbuf_new, NULL, SESMOVE, wstate);

    for(; rbuf.nodes.at(position)->move != id; ++position)
    output_node(rbuf_old, rbuf_new, rbuf.nodes.at(position), SESMOVE, wstate);

    output_node(rbuf_old, rbuf_new, NULL, SESMOVE, wstate);


    // output saved diff if is any

}
