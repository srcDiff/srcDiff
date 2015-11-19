#ifndef INCLUDED_NODE_SETS_HPP
#define INCLUDED_NODE_SETS_HPP

#include <srcdiff_vector.hpp>

#include <node_set.hpp>
#include <srcml_nodes.hpp>

#include <srcdiff_macros.hpp>

class node_sets : public srcdiff_vector<node_set> {

private:

	const srcml_nodes & nodes;

	static bool is_non_white_space(int & node_pos, const srcml_nodes & nodes, const void * context UNUSED);

public:

	typedef std::function<bool (int & node_pos, const srcml_nodes & nodes, const void * context)> node_set_filter;

	node_sets(const srcml_nodes & nodes);
	node_sets(const node_sets & sets);
	~node_sets();

	node_sets & operator=(node_sets sets);
		
	// create the node sets for shortest edit script
	node_sets(const srcml_nodes & nodes, int start, int end, const node_set_filter & filter = is_non_white_space, const void * context = 0);

};

#endif
