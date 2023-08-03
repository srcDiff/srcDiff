#ifndef INCLUDED_SRCDIFF_COMPARE
#define INCLUDED_SRCDIFF_COMPARE

#include <srcml_nodes.hpp>

namespace srcdiff_compare {

	const void * construct_list_index(int idx, const void *s, const void * context);
	int construct_compare(const void * e1, const void * e2, const void * context);

	const void * construct_node_index(int idx, const void *s, const void * context);
	int node_compare(const void * node1, const void * node2, const void * context);
	int node_compare(const std::shared_ptr<srcml_node> & node1, const std::shared_ptr<srcml_node> & node2);

	int string_compare(const void * s1, const void * s2, const void * context);
	const void * string_index(int idx, const void * s, const void * context);

}

#endif
