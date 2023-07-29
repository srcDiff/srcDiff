#include <construct_list.hpp>

construct_list::construct_list(const srcml_nodes & node_list) : node_list(node_list) {}

/** loop O(sets) */
construct_list::construct_list(const construct_list & sets) : node_list(sets.node_list) {


	for(size_type pos = 0; pos < sets.size(); ++pos) {

		push_back(sets[pos]);

	}

}

	
// create the node sets for shortest edit script
/** loop O(n) */
construct_list::construct_list(const srcml_nodes & node_list, int start, int end, const construct::construct_filter & filter, const void * context) : node_list(node_list) {

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


construct_list::~construct_list() {}

const srcml_nodes & construct_list::nodes() const {

  return node_list;

}

construct_list & construct_list::operator=(construct_list sets) {

    vec.swap(sets.vec);
    
  return *this;

}

std::ostream & operator<<(std::ostream & out, const construct_list & that) {

  for(std::size_t pos = 0, size = that.size(); pos < size; ++pos) {
    out << that.vec[pos] << '\n';
  }

  return out;

}
