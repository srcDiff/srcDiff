#ifndef INCLUDED_NODE_SET_HPP
#define INCLUDED_NODE_SET_HPP

#include <srcdiff_vector.hpp>

#include <xmlrw.hpp>

class node_set : public srcdiff_vector<int> {

private:

	const std::vector<srcml_node *> & nodes;

	static bool is_white_space(const srcml_node * node) {

	  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
	  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

	}

public:

	node_set(const std::vector<srcml_node *> & nodes) : nodes(nodes) {}

	node_set(const node_set & set) : nodes(set.nodes) {

		for(size_type pos = 0; pos < set.size(); ++pos) {

			push_back(set.data[pos]);

		}

	}

	node_set & operator=(node_set set) {

		std::swap(data, set.data);

		return *this;

	}

	node_set(const std::vector<srcml_node *> & nodes, int & start) : nodes(nodes) {

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
