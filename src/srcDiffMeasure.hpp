#ifndef INCLUDED_SRCDIFFMEASURE_HPP
#define INCLUDED_SRCDIFFMEASURE_HPP

#include <vector>

int compute_similarity(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new);

int compute_difference(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new);

int compute_similarity_old(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new);

#endif
