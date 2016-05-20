#ifndef INCLUDED_SRCDIFF_COMPARE
#define INCLUDED_SRCDIFF_COMPARE

#include <srcml_nodes.hpp>

struct diff_nodes {

  const srcml_nodes & nodes_original;
  const srcml_nodes & nodes_modified;

};

namespace srcdiff_compare {

	// diff node accessor function
	const void * node_set_index(int idx, const void *s, const void * context);

	const void * node_set_array_index(int idx, const void *s, const void * context);
	
	const void * node_index(int idx, const void *s, const void * context);

	const void * node_array_index(int idx, const void *s, const void * context);

	int node_index_compare(const void * node1, const void * node2, const void * context);

	// diff node comparison function
	int node_compare(const std::shared_ptr<srcml_node> & node1, const std::shared_ptr<srcml_node> & node2);

	// diff node comparison function
	int node_set_syntax_compare(const void * e1, const void * e2, const void * context);

	int string_compare(const void * s1, const void * s2, const void * context);

	const void * string_index(int idx, const void * s, const void * context);

}

#endif
