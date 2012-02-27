#ifndef INCLUDED_SRCDIFFCOMMENTDIFF_HPP
#define INCLUDED_SRCDIFFCOMMENTDIFFZS_HPP

#include "srcDiffTypes.hpp"
#include <vector>

NodeSets create_comment_paragraph_set(std::vector<xNodePtr> & nodes, int start, int end);

NodeSets create_comment_line_set(std::vector<xNodePtr> & nodes, int start, int end);

void output_comment_paragraph(reader_state & rbuf_old, NodeSets * node_sets_old, reader_state & rbuf_new, NodeSets * node_sets_new, writer_state & wstate);

void output_comment_line(reader_state & rbuf_old, NodeSets * node_sets_old, reader_state & rbuf_new, NodeSets * node_sets_new, writer_state & wstate);

void output_comment_word(reader_state & rbuf_old, NodeSets * node_sets_old, reader_state & rbuf_new, NodeSets * node_sets_new, writer_state & wstate);

#endif

