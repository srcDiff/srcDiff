#ifndef INCLUDED_SRCDIFFMEASURE_HPP
#define INCLUDED_SRCDIFFMEASURE_HPP

#include <vector>
#include <xmlrw.hpp>
#include <srcDiffTypes.hpp>

int compute_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * set_new);

int compute_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * set_new, int & text_old_length, int & text_new_length);

double compute_percent_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new, NodeSet * set_new);

int compute_difference(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * set_new);

int compute_similarity_old(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * set_new);

void compute_measures(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * set_new, int & similarity, int & difference, int & text_old_length, int & text_new_length);

void compute_syntax_measures(std::vector<xNodePtr> & nodes_old, NodeSet * set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * set_new, int & similarity, int & difference, int & children_old_length, int & children_new_length);

#endif
