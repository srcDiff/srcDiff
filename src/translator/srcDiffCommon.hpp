#ifndef INCLUDED_SRCDIFFSESCOMMON_HPP
#define INCLUDED_SRCDIFFSESCOMMON_HPP

#include "srcDiffTypes.hpp"

void output_common(reader_state & rbuf_old, unsigned int end_old
                   , reader_state & rbuf_new, unsigned int end_new
                   , writer_state & wstate);

void markup_common(reader_state & rbuf_old, unsigned int end_old, reader_state & rbuf_new, unsigned int end_new, writer_state & wstate);

#endif
