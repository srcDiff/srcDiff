#include "srcDiffDiff.hpp"
#include "srcDiffUtility.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffCommentDiff.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffNested.hpp"
#include <pthread.h>

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

  unsigned int similarity = compute_difference(rbuf_old.nodes, node_sets_old->at(start_old), rbuf_new.nodes, node_sets_new->at(start_new));

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

  return 3 * similarity <= 2 * min_length;

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

  diff_nodes dnodes = { rbuf_old.nodes, rbuf_new.nodes };

  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script, &dnodes);

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

  free_shortest_edit_script(edit_script);

}

int compute_similarity(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return MAX_INT;

  }

  std::vector<int> node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  std::vector<int> node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  edit * edit_script;
  shortest_edit_script(node_set_old_text.size(), (void *)&node_set_old_text, node_set_new_text.size(),
                       (void *)&node_set_new_text, node_index_compare, node_index, &edit_script, &dnodes);

  edit * edits = edit_script;
  unsigned int similarity = 0;
  int last_offset;
  for(; edits; edits = edits->next) {

    if(0 && is_change(edits)) {

      similarity += edits->length > edits->next->length ? edits->length : edits->next->length;

      edits = edits->next;

    } else {

      similarity += edits->length;

    }


  }

  similarity = ((node_set_old_text.size() + node_set_new_text.size()) - similarity);

  if(similarity <= 0)
    similarity = 10000;
  else
    similarity = 10000 / similarity;

  free_shortest_edit_script(edit_script);

  return similarity;

}

int compute_difference(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return MAX_INT;

  }

  std::vector<int> node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  std::vector<int> node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  edit * edit_script;
  shortest_edit_script(node_set_old_text.size(), (void *)&node_set_old_text, node_set_new_text.size(),
                       (void *)&node_set_new_text, node_index_compare, node_index, &edit_script, &dnodes);

  edit * edits = edit_script;
  unsigned int similarity = 0;
  int last_offset;
  for(; edits; edits = edits->next) {

    if(0 && is_change(edits)) {

      similarity += edits->length > edits->next->length ? edits->length : edits->next->length;

      edits = edits->next;

    } else {

      similarity += edits->length;

    }


  }

  free_shortest_edit_script(edit_script);

  return similarity;

}

int compute_similarity_old(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                           std::vector<int> * node_set_new) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  diff_nodes dnodes = { nodes_old, nodes_new };

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    //return olength + nlength;

    //if(node_set_syntax_compare(node_set_old, node_set_new) == 0)
    //return MIN;

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

  std::vector<int> node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  std::vector<int> node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  edit * edit_script;
  shortest_edit_script(node_set_old_text.size(), (void *)&node_set_old_text, node_set_new_text.size(),
                       (void *)&node_set_new_text, node_index_compare, node_index, &edit_script, &dnodes);

  edit * edits = edit_script;
  unsigned int similarity = 0;
  for(; edits; edits = edits->next) {

    if(is_change(edits)) {

      edits = edits->next;

    }

    ++similarity;

  }

  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, similarity);

  free_shortest_edit_script(edit_script);

  return similarity;

}

void match_differences_dynamic(std::vector<xNodePtr> & nodes_old, std::vector<std::vector<int> *> * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, std::vector<std::vector<int> *> * node_sets_new
                               , edit * edit_script, offset_pair ** matches) {

  /*

    Possible dynamic programming solution.  Sum similarities choosing left top or diagonal,
    picking smallest.  Unmatching has a high cost 1 less than a syntax mismatch.  So, it is chosen over
    a syntax mismatch.  Not sure yet, but left and diagonal probably add cost, and top might be a straight copy
    or a copy plus a unmatch.

    Errata: Now minimizing unmatched then minimizing similarity

  */

  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  /*
    fprintf(stderr, "HERE\n");

    for(int old_pos = 0; old_pos < edits->length; ++old_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_old.at(node_sets_old->at(edits->offset_sequence_one + old_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");

    for(int new_pos = 0; new_pos < edit_next->length; ++new_pos) {

    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + new_pos)->at(0))->name);

    }

    fprintf(stderr, "HERE\n");
*/

  //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

  *matches = 0;

  int olength = edits->length;
  int nlength = edit_next->length;

  size_t mem_size = olength * nlength * sizeof(difference);

  difference * differences = (difference *)malloc(mem_size);

  // still need to figure out how to track matching on each path
  for(int i = 0; i < nlength; ++i) {

    for(int j = 0; j < olength; ++j) {

      int similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + j)
                                          , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + i));

      //unsigned long long max_similarity = (unsigned long long)-1;
      int max_similarity = MAX_INT;
      int unmatched = 0;

      // check if unmatched
      if(similarity == MAX_INT) {

        similarity = 0;
        unmatched = 1;

      }

      int num_unmatched = MAX_INT;
      int direction = 0;

      bool matched = false;

      // check along x axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(j > 0) {

        //max_similarity = differences[i * olength + (j - 1)].similarity + MAX_INT;
        max_similarity = differences[i * olength + (j - 1)].similarity;
        num_unmatched = differences[i * olength + (j - 1)].num_unmatched + 1;

        matched = false;

        // may be wrong
        int temp_num_unmatched = i + j + unmatched;
        if(unmatched)
          ++temp_num_unmatched;

        //unsigned long long temp_similarity = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity < max_similarity)) {

          matched = true;

          //max_similarity = temp_similarity;
          max_similarity = similarity;
          num_unmatched = temp_num_unmatched;

        }

        direction = 1;

      }

      // check along y axis to find max difference  (Two possible either unmatch or unmatch all and add similarity
      if(i > 0) {

        // may not have been initialized in j > 0
        if(direction == 0)
          direction = 2;

        //unsigned long long temp_similarity = differences[(i - 1) * olength + j].similarity + MAX_INT;
        int temp_similarity = differences[(i - 1) * olength + j].similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + j].num_unmatched + 1;

        // may be wrong
        int temp_num_unmatched_match = i + j + unmatched;
        if(unmatched)
          ++temp_num_unmatched_match;

        int temp_matched = false;

        //unsigned long long temp_similarity_match = MAX_INT * num_unmatched + similarity;

        //if(temp_similarity_match < temp_similarity) {
        if(temp_num_unmatched_match < temp_num_unmatched || (temp_num_unmatched_match == temp_num_unmatched && similarity < temp_similarity)) {

          temp_matched = true;

          temp_similarity = similarity;
          temp_num_unmatched = temp_num_unmatched_match;

        }

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && similarity <= max_similarity)) {

          matched = temp_matched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;

          direction = 2;

        }

      }

      // go along diagonal just add similarity and unmatched
      if(i > 0 && j > 0) {

        //unsigned long long temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_similarity = differences[(i - 1) * olength + (j - 1)].similarity + similarity;
        int temp_num_unmatched = differences[(i - 1) * olength + (j - 1)].num_unmatched + unmatched;
        if(unmatched)
          ++temp_num_unmatched;

        //if(temp_similarity < max_similarity) {
        if(temp_num_unmatched < num_unmatched || (temp_num_unmatched == num_unmatched && temp_similarity < max_similarity)) {

          matched = !unmatched;

          max_similarity = temp_similarity;
          num_unmatched = temp_num_unmatched;
          direction = 3;

        }

      }

      if(direction == 1) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[i * olength + (j - 1)].marked = marked_left;
        //differences[i * olength + (j - 1)].last_similarity = last_similarity_left;

      } else if(direction == 2) {
        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
        //differences[(i - 1) * olength + j].marked = marked_top;
        //differences[(i - 1) * olength + j].last_similarity = last_similarity_top;

      } else {

        //fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

      }

      // special case starting node
      if(i == 0 && j == 0) {

        max_similarity = similarity;
        num_unmatched = unmatched;
        if(unmatched)
          ++num_unmatched;

        matched = !unmatched;
      }

      // set if marked
      if(matched) {

        differences[i * olength + j].marked = true;

      } else {

        differences[i * olength + j].marked = false;

      }

      /*
      fprintf(stderr, "HERE\n");
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, matched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, num_unmatched);
      fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, max_similarity);
      fprintf(stderr, "HERE\n");
      */

      // update structure
      differences[i * olength + j].similarity = max_similarity;
      differences[i * olength + j].num_unmatched = num_unmatched;
      differences[i * olength + j].opos = j;
      differences[i * olength + j].npos = i;
      differences[i * olength + j].direction = direction;

    }

  }

  // create match linked list
  offset_pair * last_match = NULL;

  bool * olist = (bool *)malloc(olength * sizeof(bool));
  memset(olist, 0, olength * sizeof(bool));

  bool * nlist = (bool *)malloc(nlength * sizeof(bool));
  memset(nlist, 0, nlength * sizeof(bool));

  for(int i = nlength - 1, j = olength - 1; i >= 0 || j >= 0;) {

    // only output marked and if has not already been output
    if(differences[i * olength + j].marked && !(olist[j] || nlist[i])) {

      offset_pair * match = new offset_pair;

      match->old_offset = differences[i * olength + j].opos;
      match->new_offset = differences[i * olength + j].npos;
      match->similarity = differences[i * olength + j].similarity;
      match->next = last_match;

      last_match = match;

      olist[j] = true;
      nlist[i] = true;

    }

    switch(differences[i * olength + j].direction) {

    case 0:

      --i;
      --j;

      break;

    case 1:

      --j;

      break;

    case 2:

      --i;

      break;

    case 3:

      --i;
      --j;

      break;

    default:

      break;

    }

  }

  *matches = last_match;

  free(olist);
  free(nlist);

  // free memory
  free(differences);

}

void match_differences(std::vector<xNodePtr> & nodes_old, std::vector<std::vector<int> *> * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, std::vector<std::vector<int> *> * node_sets_new
                       , edit * edit_script, offset_pair ** matches) {


  edit * edits = edit_script;
  edit * edit_next = edit_script->next;

  *matches = 0;

  struct offset_pair * curmatch = 0;

  if(edits->length > edit_next->length) {
    for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

      int min_similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + old_pos)
                                              , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + new_pos));
      int save_pos = old_pos;
      for(int pos = old_pos + 1; pos < edits->length; ++pos) {

        int similarity = 0;
        if((similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + pos)
                                            , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + new_pos))) < min_similarity) {

          old_pos = pos;
          min_similarity = similarity;
        }

      }

      int olength = node_sets_old->at(edits->offset_sequence_one + old_pos)->size();
      int nlength = node_sets_new->at(edit_next->offset_sequence_two + new_pos)->size();

      int max_size = 2 * olength;
      if(nlength > olength)
        max_size = 2 * nlength;

      if(min_similarity < max_size) {

        offset_pair * match = new offset_pair;
        match->old_offset = old_pos;
        match->new_offset = new_pos;
        match->similarity = min_similarity;
        match->next = NULL;

        if(*matches == 0) {

          *matches = match;
          curmatch = match;

        } else {

          curmatch->next = match;
          curmatch = curmatch->next;

        }
      } else {

        old_pos = save_pos - 1;

      }
    }

  } else {

    for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

      int min_similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + old_pos)
                                              , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + new_pos));

      int save_pos = new_pos;
      for(int pos = new_pos + 1; pos < edit_next->length; ++pos) {

        int similarity = 0;
        if((similarity = compute_similarity(nodes_old, node_sets_old->at(edits->offset_sequence_one + old_pos)
                                            , nodes_new, node_sets_new->at(edit_next->offset_sequence_two + pos))) < min_similarity) {

          new_pos = pos;
          min_similarity = similarity;
        }

      }

      int olength = node_sets_old->at(edits->offset_sequence_one + old_pos)->size();
      int nlength = node_sets_new->at(edit_next->offset_sequence_two + new_pos)->size();

      int max_size = 2 * olength;
      if(nlength > olength)
        max_size = 2 * nlength;

      if(min_similarity < max_size) {

        offset_pair * match = new offset_pair;
        match->old_offset = old_pos;
        match->new_offset = new_pos;
        match->similarity = min_similarity;
        match->next = NULL;

        if(*matches == 0) {

          *matches = match;
          curmatch = match;

        } else {

          curmatch->next = match;
          curmatch = curmatch->next;

        }
      } else {

        new_pos = save_pos - 1;

      }
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

  match_differences_dynamic(rbuf_old.nodes, node_sets_old, rbuf_new.nodes, node_sets_new, edit_script, &matches);

  int last_old = 0;
  int last_new = 0;

  offset_pair * matches_save = matches;

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

  output_common(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_end, SESCOMMON, wstate);

  output_white_space_statement(rbuf_old, rbuf_new, wstate);

}


void free_node_sets(std::vector<std::vector<int> *> & node_sets) {

  for(unsigned int i = 0; i < node_sets.size(); ++i) {

    delete node_sets.at(i);
  }

}
