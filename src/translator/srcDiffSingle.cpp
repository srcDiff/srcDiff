#include <srcDiffSingle.hpp>

#include <shortest_edit_script.h>
#include <srcDiffCommentDiff.hpp>
#include <srcDiffCommon.hpp>
#include <srcDiffChange.hpp>
#include <srcDiffDiff.hpp>
#include <srcDiffNested.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffWhiteSpace.hpp>

#include <string.h>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

extern xAttr diff_type;

void output_recursive_same(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
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
    NodeSets next_node_set_old
      = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    NodeSets next_node_set_new
      = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    free_node_sets(next_node_set_old);
    free_node_sets(next_node_set_new);

  }
  else {

      // collect subset of nodes
      NodeSets next_node_set_old
        = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                          , node_sets_old->at(start_old)->back());

      NodeSets next_node_set_new
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

void output_recursive_interchangeable(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

  output_white_space_all(rbuf_old, rbuf_new, wstate);

  output_node(rbuf_old, rbuf_new, &diff_old_start, SESDELETE, wstate);

  output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0)), SESDELETE, wstate);
  output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(node_sets_old->at(start_old)->at(1)), SESDELETE, wstate);

  rbuf_old.last_output += 2;

  output_node(rbuf_old, rbuf_new, &diff_new_start, SESINSERT, wstate);

  output_node(rbuf_old, rbuf_new, rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0)), SESINSERT, wstate);
  output_node(rbuf_old, rbuf_new, rbuf_new.nodes.at(node_sets_new->at(start_new)->at(1)), SESINSERT, wstate);

  rbuf_new.last_output += 2;

  // collect subset of nodes
  NodeSets next_node_set_old
    = create_node_set(rbuf_old.nodes, node_sets_old->at(start_old)->at(1)
                      , node_sets_old->at(start_old)->back());

  NodeSets next_node_set_new
    = create_node_set(rbuf_new.nodes, node_sets_new->at(start_new)->at(1)
                      , node_sets_new->at(start_new)->back());

  output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

  free_node_sets(next_node_set_old);
  free_node_sets(next_node_set_new);

  output_change(rbuf_old, rbuf_old.last_output, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  output_node(rbuf_old, rbuf_new, &diff_new_end, SESINSERT, wstate);

  output_change(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, rbuf_new.last_output, wstate);

  output_node(rbuf_old, rbuf_new, &diff_old_end, SESDELETE, wstate);

  output_white_space_statement(rbuf_old, rbuf_new, wstate);


}

void output_recursive(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

  if(strcmp((const char *)rbuf_old.nodes.at(node_sets_old->at(start_old)->front())->name, (const char *)rbuf_new.nodes.at(node_sets_new->at(start_new)->front())->name) == 0)
    output_recursive_same(rbuf_old, node_sets_old, start_old, rbuf_new, node_sets_new, start_new, wstate);
  else
    output_recursive_interchangeable(rbuf_old, node_sets_old, start_old, rbuf_new, node_sets_new, start_new, wstate);

}
