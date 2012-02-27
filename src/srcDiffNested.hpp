#ifndef INCLUDED_SRCDIFFNESTED_HPP
#define INCLUDED_SRCDIFFNESTED_HPP

#include "srcDiffTypes.hpp"
#include <vector>

bool complete_nestable(NodeSets & structure_one, std::vector<xNodePtr> & nodes_one
                       , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two);

bool is_nestable(NodeSet * structure_one, std::vector<xNodePtr> & nodes_one
                 , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two);

bool is_same_nestable(NodeSet *  structure_one, std::vector<xNodePtr> & nodes_one
                      , NodeSet * structure_two, std::vector<xNodePtr> & nodes_two);

bool has_internal_structure(NodeSet * structure, std::vector<xNodePtr> & nodes, const char * type);

void output_nested(reader_state & rbuf_old, NodeSet * structure_old
                   , reader_state & rbuf_new ,NodeSet * structure_new
                   , int operation, writer_state & wstate);

int is_block_type(NodeSet * structure, std::vector<xNodePtr> & nodes);


bool is_nest_type(NodeSet * structure, std::vector<xNodePtr> & nodes, int type_index);

#endif
