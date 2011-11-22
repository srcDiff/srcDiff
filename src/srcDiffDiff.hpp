#ifndef INCLUDED_SRCDIFFDIFF_HPP
#define INCLUDED_SRCDIFFDIFF_HPP

// create sets of nodes
std::vector<std::vector<int> *> create_node_set(std::vector<xmlNodePtr> & nodes, int start, int end);

void collect_entire_tag(std::vector<xmlNodePtr> & nodes, std::vector<int> & node_set, int & start);

#endif
