#ifndef INCLUDED_NODE_SETS_HPP
#define INCLUDED_NODE_SETS_HPP

#include <srcdiff_vector.hpp>
#include <node_set.hpp>

class node_sets : public srcdiff_vector<node_set> {

private:

	const std::vector<xNodePtr> & nodes;

	static bool is_non_white_space(const xNodePtr node, const void * context) {

	  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
	  return !((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]));

	}

	typedef bool (*node_set_filter)(const xNodePtr node, const void * context);

public:

	node_sets(const std::vector<xNodePtr> & nodes) : nodes(nodes) {}
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
	node_sets(const std::vector<xNodePtr> & nodes, int start, int end, node_set_filter filter = is_non_white_space, const void * context = 0) : nodes(nodes) {

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
