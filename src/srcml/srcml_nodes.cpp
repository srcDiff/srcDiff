#include <srcml_nodes.hpp>

void advance_to_tag(srcml_nodes & nodes,
										unsigned int & pos,
                    xmlElementType type,
                    const std::string & name,
                    const std::string & no_pass_end) {

	srcml_nodes::size_type size = nodes.size();

	int start_pos = pos;

  while(start_pos < size
  			&& !(nodes.at(start_pos)->type == (xmlElementType)XML_READER_TYPE_END_ELEMENT
  						&& nodes.at(start_pos)->name == no_pass_end)
  			&& !(nodes.at(start_pos)->type == type
  						&& nodes.at(start_pos)->name == name)
  			)
    ++start_pos; 

  pos = start_pos;

}
