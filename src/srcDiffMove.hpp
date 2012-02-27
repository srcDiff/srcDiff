#ifndef INCLUDED_SRCDIFFMOVE_HPP
#define INCLUDED_SRCDIFFMOVE_HPP

#include "srcDiffTypes.hpp"
#include "shortest_edit_script.h"

void mark_moves(reader_state & rbuf_old, NodeSets * node_sets_old
                , reader_state & rbuf_new, NodeSets * node_sets_new
                , edit * edit_script, writer_state & wstate);

void output_move(reader_state & rbuf_old, reader_state & rbuf_new, unsigned int & position, int operation, writer_state & wstate);

#endif
