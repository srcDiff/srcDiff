#include <srcDiffMeasure.hpp>

#include <srcDiffTypes.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffDiff.hpp>
#include <shortest_edit_script.h>
#include <ShortestEditScript.hpp>
#include <cstring>

void compute_ses(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                NodeSet * node_set_new, ShortestEditScript & ses, int & text_old_length, int & text_new_length) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  NodeSet node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  NodeSet node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  text_old_length = node_set_old_text.size();
  text_new_length = node_set_new_text.size();

  ses.compute((const void *)&node_set_old_text, node_set_old_text.size(), (const void *)&node_set_new_text, node_set_new_text.size());

}

void compute_ses_important_text(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                NodeSet * node_set_new, ShortestEditScript & ses, int & text_old_length, int & text_new_length) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  NodeSet node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i)))
      && strcmp((const char *)nodes_old.at(node_set_old->at(i))->content, "(") != 0
      && strcmp((const char *)nodes_old.at(node_set_old->at(i))->content, ")") != 0
      && strcmp((const char *)nodes_old.at(node_set_old->at(i))->content, ";") != 0
      && strcmp((const char *)nodes_old.at(node_set_old->at(i))->content, ",") != 0)
      node_set_old_text.push_back(node_set_old->at(i));

  NodeSet node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i)))
      && strcmp((const char *)nodes_new.at(node_set_new->at(i))->content, "(") != 0
      && strcmp((const char *)nodes_new.at(node_set_new->at(i))->content, ")") != 0
      && strcmp((const char *)nodes_new.at(node_set_new->at(i))->content, ";") != 0
      && strcmp((const char *)nodes_new.at(node_set_new->at(i))->content, ",") != 0)
      node_set_new_text.push_back(node_set_new->at(i));

  text_old_length = node_set_old_text.size();
  text_new_length = node_set_new_text.size();

  ses.compute((const void *)&node_set_old_text, node_set_old_text.size(), (const void *)&node_set_new_text, node_set_new_text.size());

}

int compute_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new, int & text_old_length, int & text_new_length) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return MAX_INT;

  }

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);

  compute_ses(nodes_old, node_set_old, nodes_new, node_set_new, ses, text_old_length, text_new_length);

  edit * edits = ses.get_script();

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    switch(edits->operation) {

      case SESDELETE :

        delete_similarity += edits->length;
        break;

      case SESINSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = text_old_length - delete_similarity;
  insert_similarity = text_new_length - insert_similarity;

  int similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity <= 0)
    similarity = 0;

  return similarity;

}

void compute_measures(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new, int & similarity, int & difference, int & text_old_length, int & text_new_length) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    similarity = MAX_INT;
    difference = MAX_INT;

    return;

  }

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);

  compute_ses_important_text(nodes_old, node_set_old, nodes_new, node_set_new, ses, text_old_length, text_new_length);

  edit * edits = ses.get_script();

  similarity = 0, difference = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    difference += edits->length;

    switch(edits->operation) {

      case SESDELETE :

        delete_similarity += edits->length;
        break;

      case SESINSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = text_old_length - delete_similarity;
  insert_similarity = text_new_length - insert_similarity;

  similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity <= 0)
    similarity = 0;

}

// create the node sets for shortest edit script
NodeSets create_significant_node_sets(std::vector<xNodePtr> & nodes, int start, int end) {

  NodeSets node_sets;

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    // skip whitespace
    if(!is_white_space(nodes.at(i)) && (!is_text(nodes.at(i))
     || (strcmp(nodes.at(i)->content, ";") != 0 && strcmp(nodes.at(i)->content, "(") != 0
      && strcmp(nodes.at(i)->content, ")") != 0 && strcmp(nodes.at(i)->content, ",") != 0))) {

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
      break;
        node_set->push_back(i);
      }

      node_sets.push_back(node_set);

    }

  }

  return node_sets;

}
void compute_syntax_measures(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new, int & similarity, int & difference, int & children_old_length, int & children_new_length) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    similarity = 0;
    difference = MAX_INT;

    return;

  }

  ShortestEditScript ses(node_set_syntax_compare, node_set_index, &dnodes);

  // collect subset of nodes
  NodeSets next_node_sets_old = create_significant_node_sets(nodes_old, node_set_old->at(1), node_set_old->back());
  NodeSets next_node_sets_new = create_significant_node_sets(nodes_new, node_set_new->at(1), node_set_new->back());
  children_old_length = next_node_sets_old.size();
  children_new_length = next_node_sets_new.size();
  int distance = ses.compute((const void *)&next_node_sets_old, children_old_length, (const void *)&next_node_sets_new, children_new_length);

  free_node_sets(next_node_sets_old);
  free_node_sets(next_node_sets_new);

  edit * edits = ses.get_script();

  similarity = 0, difference = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    difference += edits->length;

    switch(edits->operation) {

      case SESDELETE :

        delete_similarity += edits->length;
        break;

      case SESINSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = children_old_length - delete_similarity;
  insert_similarity = children_new_length - insert_similarity;

  similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity <= 0)
    similarity = 0;

}

int compute_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new) {

  int text_old_length;
  int text_new_length;

  return compute_similarity(nodes_old, node_set_old, nodes_new, node_set_new, text_old_length, text_new_length);

}

int compute_difference(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                       NodeSet * node_set_new) {


  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return MAX_INT;

  }

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);
  int text_old_length;
  int text_new_length;
  compute_ses(nodes_old, node_set_old, nodes_new, node_set_new, ses, text_old_length, text_new_length);

  edit * edits = ses.get_script();
  unsigned int similarity = 0;
  for(; edits; edits = edits->next) {

      similarity += edits->length;

  }

  return similarity;

}

double compute_percent_similarity(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new, NodeSet * node_set_new) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return 0;

  }

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);
  int text_old_length;
  int text_new_length;
  compute_ses(nodes_old, node_set_old, nodes_new, node_set_new, ses, text_old_length, text_new_length);

  edit * edits = ses.get_script();
  
  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    switch(edits->operation) {

      case SESDELETE :

        delete_similarity += edits->length;
        break;

      case SESINSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = text_old_length - delete_similarity;
  insert_similarity = text_new_length - insert_similarity;

  int similarity = delete_similarity < insert_similarity ? insert_similarity : delete_similarity;

  if(similarity <= 0)
    similarity = 0;

  return ((double)similarity) / (text_old_length < text_new_length ? text_old_length : text_new_length);

}

int compute_similarity_old(std::vector<xNodePtr> & nodes_old, NodeSet * node_set_old, std::vector<xNodePtr> & nodes_new,
                           NodeSet * node_set_new) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  diff_nodes dnodes = { nodes_old, nodes_new };

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    //return olength + nlength;

    //if(node_set_syntax_compare(node_set_old, node_set_new) == 0)
    //return MIN;

    unsigned int leftptr;
    for(leftptr = 0; leftptr < node_set_old->size() && leftptr < node_set_new->size()
          && node_compare(nodes_old.at(node_set_old->at(leftptr)), nodes_new.at(node_set_new->at(leftptr))) == 0; ++leftptr)
      ;

    unsigned int rightptr;
    for(rightptr = 1; rightptr <= node_set_old->size() && rightptr <= node_set_new->size()
          && node_compare(nodes_old.at(node_set_old->at(node_set_old->size() - rightptr)),
                          nodes_new.at(node_set_new->at(node_set_new->size() - rightptr))) == 0; ++rightptr)
      ;

    int old_diff = ((int)node_set_old->size() - rightptr) - leftptr;
    int new_diff = ((int)node_set_new->size() - rightptr) - leftptr;

    int value = ((old_diff > new_diff) ? old_diff : new_diff);

    if(value < 0)
      value = 0;

    return value;

  }

  NodeSet node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  NodeSet node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  edit * edit_script;
  shortest_edit_script((void *)&node_set_old_text, node_set_old_text.size(),
                       (void *)&node_set_new_text, node_set_new_text.size(), &edit_script, node_index_compare, node_index, &dnodes);

  edit * edits = edit_script;
  unsigned int similarity = 0;
  for(; edits; edits = edits->next) {

    if(is_change(edits)) {

      edits = edits->next;

    }

    ++similarity;

  }

  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, similarity);

  free_shortest_edit_script(edit_script);

  return similarity;

}
