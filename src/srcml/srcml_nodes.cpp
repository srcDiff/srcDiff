#include <srcml_nodes.hpp>
#include <iostream>

static xmlElementType START_TAG_TYPE = (xmlElementType)XML_READER_TYPE_ELEMENT;
static xmlElementType END_TAG_TYPE = (xmlElementType)XML_READER_TYPE_END_ELEMENT;

void advance_to_child(srcml_nodes & nodes,
                      unsigned int & pos,
                      xmlElementType type,
                      const std::string & name) {

  srcml_nodes::size_type size = nodes.size();

  int start_pos = pos;
  int depth = 0;

  while(start_pos < size) {

    if(nodes.at(start_pos)->type == END_TAG_TYPE)
      --depth;

    if(nodes.at(start_pos)->type == type
      && nodes.at(start_pos)->name == name
      && depth == 0)
      break;

    if(!nodes.at(start_pos)->is_empty && nodes.at(start_pos)->type == START_TAG_TYPE)
        ++depth;

    if(depth < 0) {

      --start_pos;
      break;

    }

    ++start_pos; 

  }

  pos = start_pos;

}
