#include <node_sets.hpp>

bool node_sets::is_non_white_space(int & node_pos, const srcml_nodes & node_list, const void * context UNUSED) {

	const std::shared_ptr<srcml_node> & node = node_list[node_pos];

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT || ((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && !node->is_white_space());

}

node_sets::node_sets(const srcml_nodes & node_list) : node_list(node_list) {}

/** loop O(sets) */
node_sets::node_sets(const node_sets & sets) : node_list(sets.node_list) {


	for(size_type pos = 0; pos < sets.size(); ++pos) {

		push_back(sets[pos]);

	}

}

	
// create the node sets for shortest edit script
/** loop O(n) */
node_sets::node_sets(const srcml_nodes & node_list, int start, int end, const node_set_filter & filter, const void * context) : node_list(node_list) {

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {
 
    // skip whitespace
    if(filter(i, node_list, context)) {

      // text is separate node if not surrounded by a tag in range
      if((xmlReaderTypes)node_list.at(i)->type == XML_READER_TYPE_TEXT || (xmlReaderTypes)node_list.at(i)->type == XML_READER_TYPE_ELEMENT) {

	      emplace_back(node_list, i);

      } else {

	      return;

      }

    }

  }

}


node_sets::~node_sets() {}

const srcml_nodes & node_sets::nodes() const {

  return node_list;

}

node_sets & node_sets::operator=(node_sets sets) {

    vec.swap(sets.vec);
    
  return *this;

}

std::ostream & operator<<(std::ostream & out, const node_sets & that) {

  for(std::size_t pos = 0, size = that.size(); pos < size; ++pos) {
    out << that.vec[pos] << '\n';
  }

  return out;

}
