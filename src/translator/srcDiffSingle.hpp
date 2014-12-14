#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcDiffTypes.hpp>
#include <vector>

void output_recursive(reader_state & rbuf_old, node_sets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, node_sets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate);

#endif
