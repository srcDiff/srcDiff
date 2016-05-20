#ifndef INCLUDED_NODE_SET_HPP
#define INCLUDED_NODE_SET_HPP

#include <srcdiff_vector.hpp>

#include <srcdiff_compare.hpp>
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

	bool operator==(const node_set & that) const {

		diff_nodes diff = { nodes(), that.nodes() };
		return srcdiff_compare::node_set_syntax_compare((const void *)this, (const void *)&that, &diff) == 0;

	}

	const srcml_nodes & nodes() const {

		return node_list;

	}

	node_set & operator=(node_set set) {

		std::swap(vec, set.vec);

		return *this;

	}

};


template<>
struct std::hash<node_set> {

  std::size_t operator()(const node_set & set) const {

	std::size_t result = 2166136261;
	for(std::size_t pos = 0, size = set.size(); pos < size; ++pos) {

		const std::shared_ptr<srcml_node> & node = set.nodes().at(set.at(pos));
		const std::string & hash_item = node->is_text() ? *node->content : node->name;
		for(std::size_t hash_pos = 0, hash_size = hash_item.size(); hash_pos < hash_size; ++hash_pos)
			result = (result * 16777619) ^ hash_item[hash_pos];

	}

    return result;

  }

};

#endif
