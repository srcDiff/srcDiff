#ifndef INCLUDED_SRCDIFFOUTPUT_HPP
#define INCLUDED_SRCDIFFOUTPUT_HPP

#include "srcDiffTypes.hpp"
#include <vector>

void output_node(reader_state & rbuf_old, reader_state & rbuf_new, const xNodePtr node, int operation, writer_state & wstate);

void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation);


void output_text_as_node(reader_state & rbuf_old, reader_state & rbuf_new, char * text, int operation
                         , writer_state & wstate);

void output_char(reader_state & rbuf_old, reader_state & rbuf_new, char character, int operation
                 , writer_state & wstate);

#endif
