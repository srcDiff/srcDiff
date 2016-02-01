#ifndef INCLUDED_NODE_SET_HPP
#define INCLUDED_NODE_SET_HPP

#include <srcdiff_vector.hpp>

#include <srcml_nodes.hpp>

#include <memory>

class node_set : public srcdiff_vector<int> {

private:

	const srcml_nodes & node_list;

	static bool is_white_space(const std::shared_ptr<srcml_node> & node) {

	  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
	  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && node->is_white_space();

	}

public:

	node_set(const srcml_nodes & node_list) : node_list(node_list) {}

	node_set(const node_set & set) : node_list(set.node_list) {

		for(size_type pos = 0; pos < set.size(); ++pos) {

			push_back(set.vec[pos]);

		}

	}

	node_set(const srcml_nodes & node_list, int & start) : node_list(node_list) {

	  if((xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_TEXT && (xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_ELEMENT) return;

	  push_back(start);

	  if(node_list.at(start)->is_empty || (xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_TEXT) return;

	  ++start;

	  // track open tags because could have same type nested
	  int is_open = 1;
	  for(; is_open; ++start) {

	    // skip whitespace
	    if(is_white_space(node_list.at(start)))
	      continue;

	    push_back(start);

	    // opening tags
	    if((xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_ELEMENT
	       && !(node_list.at(start)->is_empty))
	      ++is_open;

	    // closing tags
	    else if((xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_END_ELEMENT)
	      --is_open;

	  }

	  --start;
	}

	const srcml_nodes & nodes() const {

		return node_list;

	}

	node_set & operator=(node_set set) {

		std::swap(vec, set.vec);

		return *this;

	}

};

#endif
