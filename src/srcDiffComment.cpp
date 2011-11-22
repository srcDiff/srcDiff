/*
  Collect paragraphs
*/
std::vector<std::vector<int> *> create_comment_paragraph_set(std::vector<xmlNodePtr> & nodes, int start, int end) {

  // collect all the paragraphs separated by double newlines
  std::vector<std::vector<int> *> node_sets;
  for(int i = start; i < end; ++i) {

    // move past any starting newlines
    for(; is_new_line(nodes.at(i)); ++i)
      ;

    // collect the nodes in the paragraph
    std::vector <int> * node_set = new std::vector <int>;

    int newlines = 0;
    for(; i < end; ++i) {

      if(is_new_line(nodes.at(i)))
        ++newlines;

      if(newlines > 1)
        break;

      if(!is_white_space(nodes.at(i)))
        node_set->push_back(i);
    }

    node_sets.push_back(node_set);

  }

  return node_sets;

}

// collect lines
std::vector<std::vector<int> *> create_comment_line_set(std::vector<xmlNodePtr> & nodes, int start, int end) {

  std::vector<std::vector<int> *> node_sets;

  for(int i = start; i < end; ++i) {

    std::vector<int> * node_set = new std::vector <int>;

    for(; i < end; ++i) {

      // stop the node set at the newline
      if(is_new_line(nodes.at(i)))
        break;

      // only collect non-whitespace nodes
      if(!is_white_space(nodes.at(i)))
        node_set->push_back(i);
    }

    node_sets.push_back(node_set);

  }

  return node_sets;

}

