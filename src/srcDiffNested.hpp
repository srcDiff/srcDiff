#ifndef INCLUDED_SRCDIFFNESTED_HPP
#define INCLUDED_SRCDIFFNESTED_HPP

#include "srcDiffTypes.hpp"
#include <vector>

bool is_nestable(std::vector<int> * structure_one, std::vector<xNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xNodePtr> & nodes_two);

bool has_interal_block(std::vector<int> * structure, std::vector<xNodePtr> & nodes);

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate);

bool is_block_type(std::vector<int> * structure, std::vector<xNodePtr> & nodes);

bool is_nest_type(std::vector<int> * structure, std::vector<xNodePtr> & nodes);

#endif
