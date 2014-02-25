#ifndef INCLUDED_SRCDIFFCHANGE_HPP
#define  INCLUDED_SRCDIFFCHANGE_HPP

#include "srcDiffTypes.hpp"

void output_change_white_space(reader_state & rbuf_old, unsigned int end_old
                               , reader_state & rbuf_new, unsigned int end_new
                               , writer_state & wstate);

void output_change(reader_state & rbuf_old, unsigned int end_old
                   , reader_state & rbuf_new, unsigned int end_new
                   , writer_state & wstate);

void output_pure_operation_white_space(reader_state & rbuf_old, unsigned int end_old
                                       , reader_state & rbuf_new, unsigned int end_new
                                       , int operation, writer_state & wstate);

#endif
