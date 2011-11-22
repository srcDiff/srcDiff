// collect an entire tag from open tag to closing tag
void collect_entire_tag(std::vector<xmlNodePtr> & nodes, std::vector<int> & node_set, int & start) {

  //const char * open_node = (const char *)nodes->at(*start)->name;

  node_set.push_back(start);

  if(nodes.at(start)->extra & 0x1)
    return;

  ++start;

  // track open tags because could have same type nested
  int is_open = 1;
  for(; is_open; ++start) {

    // skip whitespace
    if(is_white_space(nodes.at(start)))
      continue;

      //      if(nodes->at(start)->type == XML_READER_TYPE_TEXT)
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(start)->content);
      //else
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(start)->name);

      node_set.push_back(start);

      // opening tags
      if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_ELEMENT
         && !(nodes.at(start)->extra & 0x1))
        ++is_open;

      // closing tags
      else if((xmlReaderTypes)nodes.at(start)->type == XML_READER_TYPE_END_ELEMENT)
        --is_open;

  }

  --start;
}

// create the node sets for shortest edit script
std::vector<std::vector<int> *> create_node_set(std::vector<xmlNodePtr> & nodes, int start, int end) {

  std::vector<std::vector<int> *> node_sets;

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    // skip whitespace
    if(!is_white_space(nodes.at(i))) {

      std::vector <int> * node_set = new std::vector <int>;

      // text is separate node if not surrounded by a tag in range
      if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_TEXT) {
        //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->content);
        node_set->push_back(i);

      } else if((xmlReaderTypes)nodes.at(i)->type == XML_READER_TYPE_ELEMENT) {

        //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->name);

        collect_entire_tag(nodes, *node_set, i);

      } else {

        // could be a closing tag, but then something should be wrong.
        // TODO: remove this and make sure it works
        node_set->push_back(i);
      }

      node_sets.push_back(node_set);

    }

  }

  return node_sets;

}


