#include "srcDiffSingle.hpp"

#include "shortest_edit_script.h"
#include "srcDiffChange.hpp"
#include "srcDiffDiff.hpp"
#include "srcDiffNested.hpp"


void output_recursive(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate) {

  if(rbuf_old.nodes.at(node_sets_old->at(start_old)->at(0))->move || rbuf_new.nodes.at(node_sets_new->at(start_new)->at(0))->move) {

    output_change_white_space(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

    return;
  }

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

      if(group_sub_elements(rbuf_old, node_sets_old, start_old, rbuf_new, node_sets_new, start_new, wstate)) {

        output_change_white_space(rbuf_old, node_sets_old->at(start_old)->back(), rbuf_new, node_sets_new->at(start_new)->back(), wstate);

      } else {

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

    }

    output_common(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

    output_node(rbuf_old, rbuf_new, &diff_common_end, SESCOMMON, wstate);

    output_white_space_statement(rbuf_old, rbuf_new, wstate);

  }

}
