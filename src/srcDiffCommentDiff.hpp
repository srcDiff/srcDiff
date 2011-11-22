#ifndef INCLUDED_SRCDIFFCOMMENTDIFF_HPP
#define INCLUDED_SRCDIFFCOMMENTDIFFZS_HPP

#include "srcDiffTypes.hpp"
#include <vector>

std::vector<std::vector<int> *> create_comment_paragraph_set(std::vector<xmlNodePtr> & nodes, int start, int end);

std::vector<std::vector<int> *> create_comment_line_set(std::vector<xmlNodePtr> & nodes, int start, int end);

void output_comment_paragraph(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate);

void output_comment_line(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate);

void output_comment_word(reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, writer_state & wstate);

#endif

