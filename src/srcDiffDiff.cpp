#include "srcDiffDiff.hpp"
#include "srcDiffUtility.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffCommentDiff.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffNested.hpp"
#include "srcDiffMeasure.hpp"
#include "srcDiffMatch.hpp"
#include "srcDiffMove.hpp"
#include <pthread.h>
#include "ShortestEditScript.hpp"

#include <string.h>
#include "Methods.hpp"

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

// collect an entire tag from open tag to closing tag
void collect_entire_tag(std::vector<xNodePtr> & nodes, std::vector<int> & node_set, int & start) {

  //const char * open_node = (const char *)nodes->at(*start)->name;

  node_set.push_back(start);

  if(nodes.at(start)->is_empty)
    return;

  ++start;

  // track open tags because could have same type nested
  int is_open = 1;
  for(; is_open; ++start) {

    // skip whitespace
    if(is_white_space(nodes.at(start)))
      continue;

    node_set.push_back(start);

    // opening tags
    if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_ELEMENT
       && !(nodes.at(start)->is_empty))
      ++is_open;

    // closing tags
    else if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_END_ELEMENT)
      --is_open;

  }

  --start;
}

// create the node sets for shortest edit script
std::vector<std::vector<int> *> create_node_set(std::vector<xNodePtr> & nodes, int start, int end) {

  std::vector<std::vector<int> *> node_sets;

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    // skip whitespace
    if(!is_white_space(nodes.at(i))) {

      std::vector <int> * node_set = new std::vector <int>;

      // text is separate node if not surrounded by a tag in range
      if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_TEXT) {
        //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->content);
        node_set->push_back(i);

      } else if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT) {

        //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->name);

        collect_entire_tag(nodes, *node_set, i);

      } else {

        // could be a closing tag, but then something should be wrong.
        // TODO: remove this and make sure it works
        node_set->push_back(i);
      }

      node_sets.push_back(node_set);

    }

  }

  return node_sets;

}

void * create_node_set_thread(void * arguments) {

  create_node_set_args & args = *(create_node_set_args *)arguments;

  args.node_sets = create_node_set(args.nodes, args.start, args.end);

  return NULL;

}

void create_node_sets(std::vector<xNodePtr> & nodes_delete, int start_old, int end_old, std::vector<std::vector<int> *> & node_set_old
                      , std::vector<xNodePtr> & nodes_insert, int start_new, int end_new, std::vector<std::vector<int> *> & node_set_new) {

  create_node_set_args args_old = { nodes_delete, start_old, end_old, node_set_old };

  pthread_t thread_old;
  if(pthread_create(&thread_old, NULL, create_node_set_thread, (void *)&args_old)) {

    exit(1);

  }

  create_node_set_args args_new = { nodes_insert, start_new, end_new, node_set_new };

  pthread_t thread_new;
  if(pthread_create(&thread_new, NULL, create_node_set_thread, (void *)&args_new)) {

    exit(1);

  }

  if(pthread_join(thread_old, NULL)) {

    exit(1);

  }

  if( pthread_join(thread_new, NULL)) {

    exit(1);

  }

}

bool go_down_a_level(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate) {


  if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "expr_stmt") != 0
     && strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "decl_stmt") != 0
     && strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "expr") != 0)
    return true;

  unsigned int similarity = compute_similarity(rbuf_old.nodes, node_sets_old->at(start_old), rbuf_new.nodes, node_sets_new->at(start_new));

  unsigned int olength = node_sets_old->at(start_old)->size();
  unsigned int nlength = node_sets_new->at(start_new)->size();

  unsigned int size_old = 0;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(i))) && !is_white_space(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(i))))
      ++size_old;

  unsigned int size_new = 0;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(i))) && !is_white_space(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(i))))
      ++size_new;

  unsigned int min_length = size_old;
  if(size_new < min_length)
    min_length = size_new;

  return 4 * similarity >= 3 * min_length;

}

bool group_sub_elements(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate) {


  if(strcmp(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "type") != 0)
     return false;

  unsigned int similarity = compute_similarity(rbuf_old.nodes, node_sets_old->at(start_old), rbuf_new.nodes, node_sets_new->at(start_new));

  unsigned int olength = node_sets_old->at(start_old)->size();
  unsigned int nlength = node_sets_new->at(start_new)->size();

  unsigned int size_old = 0;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(i))) && !is_white_space(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(i))))
      ++size_old;

  unsigned int size_new = 0;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(i))) && !is_white_space(rbuf_new.nodes.at(node_sets_new->at(start_new)->at(i))))
      ++size_new;

  unsigned int min_length = size_old;
  if(size_new < min_length)
    min_length = size_new;

  return 4 * similarity < 3 * min_length;

}

/*

  Outputs diff on each level.  First, Common areas as well as inserts and deletes
  are output directly.  One-to-one matches result in recursion on syntax match and
  direct output otherwise.  Many-to-many decides to treat parts or all of the
  change as multiple one-one or deletions followed by insert.

  Whitespace is dis-regarded during the entire process, and is incorporated on
  output before and after changes/common sections.

*/
void output_diffs(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate) {

  //fprintf(stderr, "HERE_DOUBLE\n");


  diff_nodes dnodes = { rbuf_old.nodes, rbuf_new.nodes };

  ShortestEditScript ses(node_set_syntax_compare, node_set_index, &dnodes);

  int distance = ses.compute(node_sets_old->size(), (const void *)node_sets_old, node_sets_new->size(), (const void *)node_sets_new);

  edit * edit_script = ses.get_script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  mark_moves(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edit_script, wstate);

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == SESDELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == SESINSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    edit * edit_next = edits->next;
    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // 1-1
      if(edits->length == edit_next->length && edits->length == 1
         && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
             || node_sets_old->at(edits->offset_sequence_one)->size() > 1)) {

        // syntax match
        if(node_compare(rbuf_old.nodes.at(node_sets_old->at(edits->offset_sequence_one)->at(0))
                        , rbuf_new.nodes.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))) == 0
           && (xmlReaderTypes)rbuf_old.nodes.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type != XML_READER_TYPE_TEXT) {

          if(ismethod(wstate.method, METHOD_RAW) || go_down_a_level(rbuf_old, node_sets_old, edits->offset_sequence_one
                                                                    , rbuf_new, node_sets_new, edit_next->offset_sequence_two, wstate)) {

            output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one
                             , rbuf_new, node_sets_new, edit_next->offset_sequence_two, wstate);

          } else {

            // syntax mismatch
            output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, wstate);
          }

        } else {

          if(is_nestable(node_sets_old->at(edits->offset_sequence_one)
                         , rbuf_old.nodes, node_sets_new->at(edit_next->offset_sequence_two), rbuf_new.nodes)) {

            output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two), SESINSERT, wstate);

          } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two)
                                , rbuf_new.nodes, node_sets_old->at(edits->offset_sequence_one), rbuf_old.nodes)) {

            output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)
                          , SESDELETE, wstate);

          } else {

            // syntax mismatch
            output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, wstate);
          }

        }

      } else {

        // many to many handling
        compare_many2many(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edits, wstate);
        //output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
        //                          , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);

      }

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case SESINSERT:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1,
                                          SESINSERT, wstate);


        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, SESDELETE, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

        break;
      }
    }

  }

  // determine ending position to output
  diff_end_old = rbuf_old.last_output;
  diff_end_new = rbuf_new.last_output;
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

}

void output_unmatched(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old, int end_old
                      , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new, int end_new
                      , writer_state & wstate) {

  unsigned int finish_old = rbuf_old.last_output;
  unsigned int finish_new = rbuf_new.last_output;

  if(start_old <= end_old && start_old >= 0 && end_old < (signed)node_sets_old->size()) {

    finish_old = node_sets_old->at(end_old)->back() + 1;
  }

  if(start_new <= end_new && start_new >= 0 && end_new < (signed)node_sets_new->size()) {

    finish_new = node_sets_new->at(end_new)->back() + 1;
  }

  std::vector<std::vector<int> *> slice_old;

  for(int i = start_old; i <= end_old; ++i) {

    slice_old.push_back(node_sets_old->at(i));

  }

  std::vector<std::vector<int> *> slice_new;

  for(int i = start_new; i <= end_new; ++i) {

    slice_new.push_back(node_sets_new->at(i));

  }

  if(slice_old.size() == 1 && slice_new.size() > 0
     && complete_nestable(slice_new, rbuf_new.nodes, node_sets_old->at(start_old), rbuf_old.nodes)) {

    std::vector<int> node_set;

    for(unsigned int i = 0; i < slice_new.size(); ++i) {

      for(unsigned int j = 0; j < slice_new.at(i)->size(); ++j) {

        node_set.push_back(slice_new.at(i)->at(j));

      }

    }

    output_nested(rbuf_old, node_sets_old->at(start_old), rbuf_new, &node_set, SESDELETE, wstate); 

  } else if(slice_new.size() == 1 && slice_old.size() > 0
            && complete_nestable(slice_old, rbuf_old.nodes, node_sets_new->at(start_new), rbuf_new.nodes)) {
    std::vector<int> node_set;

    for(unsigned int i = 0; i < slice_old.size(); ++i) {

      for(unsigned int j = 0; j < slice_old.at(i)->size(); ++j) {

        node_set.push_back(slice_old.at(i)->at(j));

      }

    }

    output_nested(rbuf_old, &node_set, rbuf_new, node_sets_new->at(start_new), SESINSERT, wstate); 

  } else
    output_change_white_space(rbuf_old, finish_old, rbuf_new, finish_new, wstate);

}

void compare_many2many(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, writer_state & wstate) {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  offset_pair * matches = NULL;

  std::vector<std::pair<int, int> > old_moved;

  for(unsigned int i = 0; i < node_sets_old->size(); ++i) {

    if(rbuf_old.nodes.at(node_sets_old->at(i)->at(0))->move)
      old_moved.push_back(std::pair<int, int>(SESMOVE, 0));
    else
      old_moved.push_back(std::pair<int, int>(SESDELETE, 0));

  }

  std::vector<std::pair<int, int> > new_moved;

  for(unsigned int i = 0; i < node_sets_new->size(); ++i) {

    if(rbuf_new.nodes.at(node_sets_new->at(i)->at(0))->move)
      new_moved.push_back(std::pair<int, int>(SESMOVE, 0));
    else
      new_moved.push_back(std::pair<int, int>(SESINSERT, 0));

  } 

  match_differences_dynamic(rbuf_old.nodes, node_sets_old, rbuf_new.nodes, node_sets_new, edit_script, &matches);

  int last_old = 0;
  int last_new = 0;

  offset_pair * matches_save = matches;

  /*

  for(; matches; matches = matches->next) {

  old_moved.at(matches->old_offset) = SESCOMMON;
  new_moved.at(matches->new_offset) = SESCOMMON;

  */

  for(; matches; matches = matches->next) {

    // output diffs until match
    output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                     edits->offset_sequence_one + matches->old_offset - 1,
                     rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                     , edit_next->offset_sequence_two + matches->new_offset - 1, wstate);

    if(node_compare(rbuf_old.nodes.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))
                    , rbuf_new.nodes.at(node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->at(0))) == 0
       && (xmlReaderTypes)rbuf_old.nodes.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))->type != XML_READER_TYPE_TEXT) {

      if(ismethod(wstate.method, METHOD_RAW) || go_down_a_level(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset
                                                                , rbuf_new, node_sets_new, edit_next->offset_sequence_two + matches->new_offset, wstate)) {

        output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset
                         , rbuf_new, node_sets_new, edit_next->offset_sequence_two + matches->new_offset, wstate);

      } else {

        // syntax mismatch
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1, wstate);
      }

    } else {

      if(is_nestable(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)
                     , rbuf_old.nodes, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), rbuf_new.nodes)) {

        output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), SESINSERT, wstate);

      } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)
                            , rbuf_new.nodes, node_sets_old->at(edits->offset_sequence_one + matches->old_offset), rbuf_old.nodes)) {

        output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), SESDELETE, wstate);

      } else {

        // syntax mismatch
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1, wstate);
      }

    }

    last_old = matches->old_offset + 1;
    last_new = matches->new_offset + 1;

  }

  // output diffs until match
  output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                   edits->offset_sequence_one + edits->length - 1,
                   rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                   , edit_next->offset_sequence_two + edit_next->length - 1, wstate);

  for(; matches_save;) {

    offset_pair * old_match = matches_save;
    matches_save = matches_save->next;
    delete old_match;

  }

}

void output_recursive(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

  if(is_same_nestable(node_sets_old->at(start_old)
                      , rbuf_old.nodes, node_sets_new->at(start_new), rbuf_new.nodes)) {

    output_nested(rbuf_old, node_sets_old->at(start_old), rbuf_new, node_sets_new->at(start_new), SESINSERT, wstate);

  } else if(is_same_nestable(node_sets_new->at(start_new)
                                , rbuf_new.nodes, node_sets_old->at(start_old), rbuf_old.nodes)) {

    output_nested(rbuf_old, node_sets_old->at(start_old), rbuf_new, node_sets_new->at(start_new)
                          , SESDELETE, wstate);

  } else {


  output_white_space_all(rbuf_old, rbuf_new, wstate);
  //markup_common(rbuf_old, node_sets_old->at(start_old)->at(0), rbuf_new, node_sets_new->at(start_new)->at(0), wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_start, SESCOMMON, wstate);

  output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), SESCOMMON, wstate);

  ++rbuf_old.last_output;
  ++rbuf_new.last_output;

  // compare subset of nodes
  if(strcmp((const char *)rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    free_node_sets(next_node_set_old);
    free_node_sets(next_node_set_new);

  }
  else {

    if(group_sub_elements(rbuf_old, node_sets_old, start_old, rbuf_new, node_sets_new, start_new, wstate)) {

      output_change_white_space(rbuf_old, node_sets_old->at(start_old)->back(), rbuf_new, node_sets_new->at(start_new)->back(), wstate);

    } else {

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->back());

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->back());

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    free_node_sets(next_node_set_old);
    free_node_sets(next_node_set_new);

    }

  }

  output_common(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_end, SESCOMMON, wstate);

  output_white_space_statement(rbuf_old, rbuf_new, wstate);

          }

}


void free_node_sets(std::vector<std::vector<int> *> & node_sets) {

  for(unsigned int i = 0; i < node_sets.size(); ++i) {

    delete node_sets.at(i);
  }

}
