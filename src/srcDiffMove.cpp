#include "srcDiffMove.hpp"
#include <vector>
#include <map>
#include "xmlrw.hpp"
#include "srcDiffTypes.hpp"
#include "shortest_edit_script.h"

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

void check_move(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                , edit * edit_script, writer_state & wstate) {

  if(move_id == 0)
    ++move_id;

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


}

