#ifndef INCLUDED_NODE_SET_HPP
#define INCLUDED_NODE_SET_HPP

#include <vector>

#include <xmlrw.hpp>




class node_set : public std::vector<int> {

private:

	//std::vector<xNodePtr> & nodes;
	std::vector<xNodePtr> nodes;

	static bool is_white_space(const xNodePtr node) {

	  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
	  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

	}

public:

	node_set() {}

	node_set(std::vector<xNodePtr> & nodes, int & start) {

	if((xmlReaderTypes)nodes.at(start)->type != XML_READER_TYPE_TEXT && (xmlReaderTypes)nodes.at(start)->type != XML_READER_TYPE_ELEMENT) return;

	  push_back(start);

	  if(nodes.at(start)->is_empty || (xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_TEXT) return;

	  ++start;

	  // track open tags because could have same type nested
	  int is_open = 1;
	  for(; is_open; ++start) {

	    // skip whitespace
	    if(is_white_space(nodes.at(start)))
	      continue;

	    push_back(start);

	    // opening tags
	    if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_ELEMENT
	       && !(nodes.at(start)->is_empty))
	      ++is_open;

	    // closing tags
	    else if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_END_ELEMENT)
	      --is_open;

	  }

	  --start;
	}

};

#endif
