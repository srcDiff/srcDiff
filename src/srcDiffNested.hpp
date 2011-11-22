#ifndef INCLUDED_SRCDIFFNESTED_HPP
#define INCLUDED_SRCDIFFNESTED_HPP

bool is_nestable(std::vector<int> * structure_one, std::vector<xmlNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xmlNodePtr> & nodes_two);

bool has_interal_block(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes);

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate);

bool is_block_type(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes);

bool is_nest_type(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  for(int i = 0; nest_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nest_types[i]) == 0)
      return true;

  return false;
}


#endif
