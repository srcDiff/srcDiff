#include <srcDiffDiff.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffCommon.hpp>
#include <srcDiffChange.hpp>
#include <srcDiffCommentDiff.hpp>
#include <srcDiffWhiteSpace.hpp>
#include <srcDiffNested.hpp>
#include <srcDiffMeasure.hpp>
#include <srcDiffMatch.hpp>
#include <srcDiffMove.hpp>
#include <srcDiffMany.hpp>
#include <srcDiffSingle.hpp>
#include <pthread.h>
#include <ShortestEditScript.hpp>

#include <string.h>
#include <Methods.hpp>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

// collect an entire tag from open tag to closing tag
void collect_entire_tag(std::vector<xNodePtr> & nodes, NodeSet & node_set, int & start) {

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
NodeSets create_node_set(std::vector<xNodePtr> & nodes, int start, int end) {

  NodeSets node_sets;

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
      break;
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

void create_node_sets(std::vector<xNodePtr> & nodes_delete, int start_old, int end_old, NodeSets & node_set_old
                      , std::vector<xNodePtr> & nodes_insert, int start_new, int end_new, NodeSets & node_set_new) {

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

  if(pthread_join(thread_new, NULL)) {

    exit(1);

  }

}

bool go_down_a_level(reader_state & rbuf_old, NodeSets * node_sets_old
                     , unsigned int start_old
                     , reader_state & rbuf_new, NodeSets * node_sets_new
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

bool group_sub_elements(reader_state & rbuf_old, NodeSets * node_sets_old
                        , unsigned int start_old
                        , reader_state & rbuf_new, NodeSets * node_sets_new
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
void output_diffs(reader_state & rbuf_old, NodeSets * node_sets_old, reader_state & rbuf_new, NodeSets * node_sets_new, writer_state & wstate) {

  //fprintf(stderr, "HERE_DOUBLE\n");

  diff_nodes dnodes = { rbuf_old.nodes, rbuf_new.nodes };

  ShortestEditScript ses(node_set_syntax_compare, node_set_index, &dnodes);

  int distance = ses.compute((const void *)node_sets_old, node_sets_old->size(), (const void *)node_sets_new, node_sets_new->size());

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
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    } else if(edits->operation == SESINSERT && edits->offset_sequence_one != 0 && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(edits->offset_sequence_two - 1)->back() + 1;

    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);
    // detect and change
    edit * edit_next = edits->next;

    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // 1-1
      if(!is_move(node_sets_old->at(edits->offset_sequence_one), rbuf_old.nodes)
         && !is_move(node_sets_new->at(edit_next->offset_sequence_two), rbuf_new.nodes)
         && edits->length == edit_next->length && edits->length == 1
         && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
             || node_sets_new->at(edit_next->offset_sequence_two)->size() > 1)) {

        if(is_nestable(node_sets_old->at(edits->offset_sequence_one)
                         , rbuf_old.nodes, node_sets_new->at(edit_next->offset_sequence_two), rbuf_new.nodes)) {

            set_nestable(node_sets_old, rbuf_old.nodes, edits->offset_sequence_one, edits->offset_sequence_one + edits->length
                        , node_sets_new, rbuf_new.nodes, edit_next->offset_sequence_two, edit_next->offset_sequence_two + edit_next->length);

          } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two)
                                , rbuf_new.nodes, node_sets_old->at(edits->offset_sequence_one), rbuf_old.nodes)) {

            set_nestable(node_sets_old, rbuf_old.nodes, edits->offset_sequence_one, edits->offset_sequence_one + edits->length
                        , node_sets_new, rbuf_new.nodes, edit_next->offset_sequence_two, edit_next->offset_sequence_two + edit_next->length);

          }

      } else if(edits->length == 1 || edit_next->length == 1){

        int start_nest_old, end_nest_old, start_nest_new, end_nest_new;
        check_nestable(node_sets_old, rbuf_old.nodes, edits->offset_sequence_one, edits->offset_sequence_one + edits->length
                        , node_sets_new, rbuf_new.nodes, edit_next->offset_sequence_two, edit_next->offset_sequence_two + edit_next->length
                        , start_nest_old, end_nest_old, start_nest_new, end_nest_new);

        set_nestable(node_sets_old, rbuf_old.nodes, start_nest_old, end_nest_old
                        , node_sets_new, rbuf_new.nodes, start_nest_new, end_nest_new);
      }

      // many to many handling
      output_many(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edits, wstate);

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
        last_diff_old = edits->offset_sequence_one;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case SESDELETE:

        //fprintf(stderr, "HERE\n");
        output_pure_operation_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                          , rbuf_new, 0, SESDELETE, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two;

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

void free_node_sets(NodeSets & node_sets) {

  for(unsigned int i = 0; i < node_sets.size(); ++i) {

    delete node_sets.at(i);
  }

}
