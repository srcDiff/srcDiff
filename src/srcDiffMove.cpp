#include "srcDiffMove.hpp"
#include <vector>
#include <map>

static int move_id;

void add_construct(std::map<std::string, std::vector<std::pair<int, int> > > & construct
                   , std::vector<std::vector<int> *> & node_sets, std::vector<xNodePtr> & nodes
                   , int offset, int operation) {

  std::string tag = nodes.at(node_sets.at(offset)->at(0));

  if(construct.count(tag) == 0) {

    construct[tag] = std::vector<std::pair<int, int> >();

  }

  construct[tag].push_back(std::pair<int, int >(offset, operation))

}

void check_move(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, writer_state & wstate) {

  std::map<std::string, std::vector<std::pair<int, int> > > constructs;

  for(edit * edits = edit_script; edits; edits = edits->next) {

    /*
    if(is_change(edits)) {


    }
    */

    switch(edits->operation) {

    case SESINSERT :

      for(int i = 0; i < edits->length; ++i) {

        add_construct(constructs, node_sets_new, rbuf_new.nodes, edits->offset_sequence_two, SESINSERT);


      }

      break;

    case SESDELETE :

      for(int i = 0; i < edits->length; ++i) {


      }

      break;


    }


  }

  /*
  std::vector<std::vector<int> *> function_old;

  for(unsigned int i = 0; i < node_sets_old->size(); ++i) {

    if(strcmp(rbuf_old.nodes.at(node_sets_old->at(i)->at(0))->name, "function") != 0)
       continue;

    function_old.push_back(node_sets_old->at(i));

  }

  std::vector<std::vector<int> *> function_new;

  for(unsigned int i = 0; i < node_sets_new->size(); ++i) {

    if(strcmp(rbuf_new.nodes.at(node_sets_new->at(i)->at(0))->name, "function") != 0)
      continue;

    function_new.push_back(node_sets_new->at(i));

  }

  offset_pair * matches = NULL;

  match_differences_dynamic_unordered(rbuf_old.nodes, &function_old, rbuf_new.nodes, &function_new, edit_script, &matches);

  offset_pair * matches_save = matches;

  static int count = 0;

  int start_count = count;

  for(; matches; matches = matches->next) {

    unsigned int min_size = function_old.at(matches->old_offset)->size();
    if(function_new.at(matches->new_offset)->size() < min_size)
      min_size = function_new.at(matches->new_offset)->size();

    //if(compute_similarity(rbuf_old.nodes, function_old.at(matches->old_offset)
    //                    , rbuf_new.nodes, function_new.at(matches->new_offset)) * 10 > min_size * 5) {

      output_change_white_space(rbuf_old, function_old.at(matches->old_offset)->back() + 1, rbuf_new, rbuf_new.last_output, wstate);
      ++count;

      //    }

  }

  matches = matches_save;

  for(; matches; matches = matches->next) {

    unsigned int min_size = function_old.at(matches->old_offset)->size();
    if(function_new.at(matches->new_offset)->size() < min_size)
      min_size = function_new.at(matches->new_offset)->size();

    //if(compute_similarity(rbuf_old.nodes, function_old.at(matches->old_offset)
    //                    , rbuf_new.nodes, function_new.at(matches->new_offset)) * 10 > min_size * 9) {

      output_change_white_space(rbuf_old, rbuf_old.last_output, rbuf_new, function_new.at(matches->new_offset)->back() + 1, wstate);
      ++start_count;
      //}

  }

  for(; matches_save;) {

    offset_pair * old_match = matches_save;
    matches_save = matches_save->next;
    delete old_match;

  }
  */
}

