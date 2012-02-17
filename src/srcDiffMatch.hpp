#ifndef INCLUDED_SRCDIFFMATCH_HPP
#define INCLUDED_SRCDIFFMATCH_HPP

#include <vector>
#include "xmlrw.hpp"
#include "srcDiffDiff.hpp"
#include "shortest_edit_script.h"

void match_differences_dynamic(std::vector<xNodePtr> & nodes_old, std::vector<std::vector<int> *> * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, std::vector<std::vector<int> *> * node_sets_new
                               , edit * edit_script, offset_pair ** matches);

void match_differences_dynamic_unordered(std::vector<xNodePtr> & nodes_old, std::vector<std::vector<int> *> * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, std::vector<std::vector<int> *> * node_sets_new
                               , edit * edit_script, offset_pair ** matches);

void match_differences(std::vector<xNodePtr> & nodes_old, std::vector<std::vector<int> *> * node_sets_old
                               , std::vector<xNodePtr> & nodes_new, std::vector<std::vector<int> *> * node_sets_new
                               , edit * edit_script, offset_pair ** matches);

#endif
