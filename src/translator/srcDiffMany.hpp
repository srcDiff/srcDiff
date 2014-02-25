#ifndef INCLUDED_SRCDIFFMANY_HPP
#define INCLUDED_SRCDIFFMANY_HPP

#include <srcDiffTypes.hpp>
#include <shortest_edit_script.h>
#include <xmlrw.hpp>
#include <vector>

typedef std::vector<IntPairs> Moves;

void output_unmatched(reader_state & rbuf_old, NodeSets * node_sets_old
                      , int start_old, int end_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , int start_new, int end_new
                      , writer_state & wstate);

Moves determine_operations(reader_state & rbuf_old, NodeSets * node_sets_old
                           , reader_state & rbuf_new, NodeSets * node_sets_new
                           , edit * edit_script, writer_state & wstate);

void output_many(reader_state & rbuf_old, NodeSets * node_sets_old
                       , reader_state & rbuf_new, NodeSets * node_sets_new
                       , edit * edit_script, writer_state & wstate);

#endif
