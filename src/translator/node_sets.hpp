#ifndef INCLUDED_NODE_SETS_HPP
#define INCLUDED_NODE_SETS_HPP

#include <vector>
#include <node_set.hpp>

class node_sets : public std::vector<node_set *> {

private:

	std::vector<xNodePtr> & nodes;

	static bool is_non_white_space(const xNodePtr node) {

	  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
	  return !((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]));

	}

	typedef bool (*node_set_filter)(const xNodePtr node);

public:

	node_sets(std::vector<xNodePtr> & nodes) : nodes(nodes) {}
	node_sets(const node_sets & sets) : nodes(sets.nodes) {}

	node_sets & operator=(const node_sets & sets) {

		std::vector<node_set *>::operator=(sets);

		return *this;

	}
		
	// create the node sets for shortest edit script
	node_sets(std::vector<xNodePtr> & nodes, int start, int end, node_set_filter filter = is_non_white_space) : nodes(nodes) {

	  // runs on a subset of base array
	  for(int i = start; i < end; ++i) {

	    // skip whitespace
	    if(filter(nodes.at(i))) {

	      // text is separate node if not surrounded by a tag in range
	      if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_TEXT || (xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT) {

		      node_set * set = new node_set(nodes, i);
		      push_back(set);

	      } else {

		      return;

	      }

	    }

	  }

	}

};

#endif
