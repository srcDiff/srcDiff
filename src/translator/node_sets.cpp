#include <node_sets.hpp>

bool node_sets::is_non_white_space(int & node_pos, const srcml_nodes & nodes, const void * context UNUSED) {

	const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return !((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && node->is_white_space());

}

node_sets::node_sets(const srcml_nodes & nodes) : nodes(nodes) {}

node_sets::node_sets(const node_sets & sets) : nodes(sets.nodes) {


	for(size_type pos = 0; pos < sets.size(); ++pos) {

		push_back(sets[pos]);

	}

}

node_sets::~node_sets() {}

node_sets & node_sets::operator=(node_sets sets) {

    std::swap(data, sets.data);
    
	return *this;

}
	
// create the node sets for shortest edit script
node_sets::node_sets(const srcml_nodes & nodes, int start, int end, const node_set_filter & filter, const void * context) : nodes(nodes) {

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    // skip whitespace
    if(filter(i, nodes, context)) {

      // text is separate node if not surrounded by a tag in range
      if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_TEXT || (xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT) {

	      emplace_back(nodes, i);

      } else {

	      return;

      }

    }

  }

}
