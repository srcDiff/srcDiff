
void output_unmatched(reader_state & rbuf_old, NodeSets * node_sets_old
                      , int start_old, int end_old
                      , reader_state & rbuf_new, NodeSets * node_sets_new
                      , int start_new, int end_new
                      , writer_state & wstate);

void compare_many2many(reader_state & rbuf_old, NodeSets * node_sets_old
                       , reader_state & rbuf_new, NodeSets * node_sets_new
                       , edit * edit_script, writer_state & wstate);
