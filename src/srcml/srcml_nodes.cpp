#include <srcml_nodes.hpp>

void advance_to_tag(srcml_nodes & nodes,
										unsigned int & pos,
                    xmlElementType type,
                    const std::string & name,
                    std::size_t end_pos) {

	srcml_nodes::size_type size = nodes.size();
  std::size_t end = end_pos < size ? end_pos : size;

	int start_pos = pos;

  while(start_pos < end) {

      if(nodes.at(start_pos)->type == type
              && nodes.at(start_pos)->name == name)
        break;

    ++start_pos; 

  }

  pos = start_pos;

}
