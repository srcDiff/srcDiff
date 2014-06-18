#ifndef INCLUDED_SRCDIFFMEASURE_HPP
#define INCLUDED_SRCDIFFMEASURE_HPP

#include <vector>
#include <xmlrw.hpp>
#include <srcDiffTypes.hpp>

int compute_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new);

double compute_percent_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new, NodeSet * node_set_new);

int compute_difference(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new);

int compute_similarity_old(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new);

#endif
