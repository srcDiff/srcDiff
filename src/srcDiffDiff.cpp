#include "srcDiffDiff.hpp"
#include "srcDiffUtility.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffCommentDiff.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffNested.hpp"

#include <string.h>

// global structures defined in main
extern std::vector<xNode *> nodes_old;
extern std::vector<xNode *> nodes_new;

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

  if(nodes.at(start)->extra & 0x1)
    return;

  ++start;

  // track open tags because could have same type nested
  int is_open = 1;
  for(; is_open; ++start) {

    // skip whitespace
    if(is_white_space(nodes.at(start)))
      continue;

    //      if(nodes->at(start)->type == XML_READER_TYPE_TEXT)
    //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(start)->content);
    //else
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(start)->name);

    node_set.push_back(start);

    // opening tags
    if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_ELEMENT
       && !(nodes.at(start)->extra & 0x1))
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

bool go_down_a_level(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                     , unsigned int start_new
                     , writer_state & wstate) {


  if(strcmp(nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "expr_stmt") != 0
     && strcmp(nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "decl_stmt") != 0
     && strcmp(nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "expr") != 0
     && strcmp(nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "init") != 0)
  return true;

  int olength = node_sets_old->at(start_old)->size();
  int nlength = node_sets_new->at(start_new)->size();

  int similarity = compute_similarity(node_sets_old->at(start_old), node_sets_new->at(start_new));

  int max_length = nlength;
  if(nlength > olength)
    max_length = nlength;

  return 3 * similarity < max_length;

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

  edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

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
        if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))
                        , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))) == 0
           && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type != XML_READER_TYPE_TEXT) {

          if(go_down_a_level(rbuf_old, node_sets_old, edits->offset_sequence_one
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
                         , nodes_old, node_sets_new->at(edit_next->offset_sequence_two), nodes_new)) {

            output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two), INSERT, wstate);

          } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two)
                                , nodes_new, node_sets_old->at(edits->offset_sequence_one), nodes_old)) {

            output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)
                          , DELETE, wstate);

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

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, 0
                                          , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1,
                                          INSERT, wstate);


        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, DELETE, wstate);

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

  free_shortest_edit_script(edit_script);

}


int compute_similarity(std::vector<int> * node_set_old, std::vector<int> * node_set_new) {

  if(node_set_syntax_compare(node_set_old, node_set_new) == 0)
    return MIN;


  unsigned int leftptr;
  for(leftptr = 0; leftptr < node_set_old->size() && leftptr < node_set_new->size()
        && node_compare(nodes_old.at(node_set_old->at(leftptr)), nodes_new.at(node_set_new->at(leftptr))) == 0; ++leftptr)
    ;

  unsigned int rightptr;
  for(rightptr = 1; rightptr <= node_set_old->size() && rightptr <= node_set_new->size()
        && node_compare(nodes_old.at(node_set_old->at(node_set_old->size() - rightptr)),
                        nodes_new.at(node_set_new->at(node_set_new->size() - rightptr))) == 0; ++rightptr)
    ;

  int old_diff = ((int)node_set_old->size() - rightptr) - leftptr;
  int new_diff = ((int)node_set_new->size() - rightptr) - leftptr;

  int value = ((old_diff > new_diff) ? old_diff : new_diff);

  if(value < 0)
    value = 0;

  return value;
}

void match_differences(std::vector<std::vector<int> *> * node_sets_old
                       , std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, offset_pair ** matches) {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  if(edits->length > edit_next->length) {
    for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

      // TODO: set to first or positive infinity or MAX_INT or whatever it is called
      int min_similarity = 65535;
      for(int pos = old_pos; pos < edits->length; ++pos) {

        int similarity = 0;
        if((similarity = compute_similarity(node_sets_old->at(edits->offset_sequence_one + pos)
                                            , node_sets_new->at(edit_next->offset_sequence_two + new_pos))) < min_similarity) {

          old_pos = pos;
          min_similarity = similarity;
        }

      }

      offset_pair * match = new offset_pair;
      match->old_offset = old_pos;
      match->new_offset = new_pos;
      match->similarity = min_similarity;
      match->next = NULL;

      if(new_pos == 0)
        *matches = match;
      else
        (*matches)->next = match;

    }

  } else {

    for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

      // TODO: set to first or positive infinity or MAX_INT or whatever it is called
      int min_similarity = 65535;
      for(int pos = new_pos; pos < edit_next->length; ++pos) {

        int similarity = 0;
        if((similarity = compute_similarity(node_sets_old->at(edits->offset_sequence_one + old_pos)
                                            , node_sets_new->at(edit_next->offset_sequence_two + pos))) < min_similarity) {

          new_pos = pos;
          min_similarity = similarity;
        }

      }

      offset_pair * match = new offset_pair;
      match->old_offset = old_pos;
      match->new_offset = new_pos;
      match->similarity = min_similarity;
      match->next = NULL;

      if(old_pos == 0)
        *matches = match;
      else
        (*matches)->next = match;

    }

  }

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

  output_change_white_space(rbuf_old, finish_old, rbuf_new, finish_new, wstate);

}

void compare_many2many(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, writer_state & wstate) {

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  offset_pair * matches = NULL;

  match_differences(node_sets_old, node_sets_new, edit_script, &matches);

  int last_old = 0;
  int last_new = 0;

  offset_pair * matches_save = matches;

  for(; matches; matches = matches->next) {

    // output diffs until match
    output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                     edits->offset_sequence_one + matches->old_offset - 1,
                     rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                     , edit_next->offset_sequence_two + matches->new_offset - 1, wstate);

    // correct could only be whitespace
    if(matches->similarity == MIN) {


      output_common(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1

                    , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1
                    , wstate);

    } else if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))
                           , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->at(0))) == 0
              && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))->type != XML_READER_TYPE_TEXT) {

      if(go_down_a_level(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset
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
                     , nodes_old, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), nodes_new)) {

        output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), INSERT, wstate);

      } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)
                            , nodes_new, node_sets_old->at(edits->offset_sequence_one + matches->old_offset), nodes_old)) {

        output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), DELETE, wstate);

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


  output_white_space_all(rbuf_old, rbuf_new, wstate);
  //markup_common(rbuf_old, node_sets_old->at(start_old)->at(0), rbuf_new, node_sets_new->at(start_new)->at(0), wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);

  output_node(rbuf_old, rbuf_new, nodes_old.at(node_sets_old->at(start_old)->at(0)), COMMON, wstate);

  ++rbuf_old.last_output;
  ++rbuf_new.last_output;

  // compare subset of nodes

  if(strcmp((const char *)nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    free_node_sets(next_node_set_old);
    free_node_sets(next_node_set_new);

  }
  else {

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->back());

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->back());

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    free_node_sets(next_node_set_old);
    free_node_sets(next_node_set_new);

  }

  output_common(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

  output_white_space_statement(rbuf_old, rbuf_new, wstate);

}


void free_node_sets(std::vector<std::vector<int> *> & node_sets) {

  for(unsigned int i = 0; i < node_sets.size(); ++i) {

    delete node_sets.at(i);
  }

}
