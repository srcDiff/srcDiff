#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcDiffTypes.hpp>
#include <vector>

void output_recursive(reader_state & rbuf_old, NodeSets * node_sets_old
                      , unsigned int start_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , unsigned int start_new
                      , writer_state & wstate);

#endif
