#ifndef INCLUDED_SRCDIFFOUTPUT_HPP
#define INCLUDED_SRCDIFFOUTPUT_HPP

#include "srcDiffTypes.hpp"
#include <vector>

void output_node(reader_state & rbuf_old, reader_state & rbuf_new, xmlNodePtr node, int operation, writer_state & wstate);

void update_diff_stack(std::vector<diff_set *> & open_diffs, xmlNodePtr node, int operation);


void output_text_as_node(reader_state & rbuf_old, reader_state & rbuf_new, xmlChar * text, int operation, writer_state & wstate);

#endif
