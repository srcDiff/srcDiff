bool is_nestable(std::vector<int> * structure_one, std::vector<xmlNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xmlNodePtr> & nodes_two);

bool has_interal_block(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes);

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate);

