#ifndef INCLUDED_NODE_SETS_HPP
#define INCLUDED_NODE_SETS_HPP

#include <srcdiff_vector.hpp>

#include <node_set.hpp>

#include <srcdiff_macros.hpp>

class node_sets : public srcdiff_vector<node_set> {

private:

	const srcml_nodes & nodes;

	static bool is_non_white_space(const std::shared_ptr<srcml_node> & node, const void * context UNUSED) {

	  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
	  return !((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && node->is_white_space());

	}

	typedef bool (*node_set_filter)(const std::shared_ptr<srcml_node> & node, const void * context);

public:

	node_sets(const srcml_nodes & nodes) : nodes(nodes) {}
	node_sets(const node_sets & sets) : nodes(sets.nodes) {


		for(size_type pos = 0; pos < sets.size(); ++pos) {

			push_back(sets[pos]);

		}

	}

	node_sets & operator=(node_sets sets) {

        std::swap(data, sets.data);
        
		return *this;

	}

	~node_sets() {}
		
	// create the node sets for shortest edit script
	node_sets(const srcml_nodes & nodes, int start, int end, node_set_filter filter = is_non_white_space, const void * context = 0) : nodes(nodes) {

	  // runs on a subset of base array
	  for(int i = start; i < end; ++i) {

	    // skip whitespace
	    if(filter(nodes.at(i), context)) {

	      // text is separate node if not surrounded by a tag in range
	      if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_TEXT || (xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT) {

		      emplace_back(nodes, i);

	      } else {

		      return;

	      }

	    }

	  }

	}

};

#endif
