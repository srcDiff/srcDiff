#include <srcdiff_measure.hpp>

#include <srcdiff_constants.hpp>
#include <srcDiffUtility.hpp>
#include <srcdiff_diff.hpp>
#include <cstring>

srcdiff_measure::srcdiff_measure(std::vector<xNodePtr> & nodes_old, std::vector<xNodePtr> & nodes_new, node_set * set_old, node_set * set_new) 
  : nodes_old(nodes_old), nodes_new(nodes_new), set_old(set_old), set_new(set_new) {}

void srcdiff_measure::compute_ses(ShortestEditScript & ses, int & text_old_length, int & text_new_length) {

  unsigned int olength = set_old->size();
  unsigned int nlength = set_new->size();

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(set_new->at(0))->name);

  node_set set_old_text(nodes_old);

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(set_old->at(i))) && !is_white_space(nodes_old.at(set_old->at(i))))
      set_old_text.push_back(set_old->at(i));

  node_set set_new_text(nodes_new);

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(set_new->at(i))) && !is_white_space(nodes_new.at(set_new->at(i))))
      set_new_text.push_back(set_new->at(i));

  text_old_length = set_old_text.size();
  text_new_length = set_new_text.size();

  ses.compute((const void *)&set_old_text, set_old_text.size(), (const void *)&set_new_text, set_new_text.size());

}

void srcdiff_measure::compute_ses_important_text(ShortestEditScript & ses, int & text_old_length, int & text_new_length) {

  unsigned int olength = set_old->size();
  unsigned int nlength = set_new->size();

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(set_new->at(0))->name);

  node_set set_old_text(nodes_old);

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(set_old->at(i))) && !is_white_space(nodes_old.at(set_old->at(i)))
      && strcmp((const char *)nodes_old.at(set_old->at(i))->content, "(") != 0
      && strcmp((const char *)nodes_old.at(set_old->at(i))->content, ")") != 0
      && strcmp((const char *)nodes_old.at(set_old->at(i))->content, ";") != 0
      && strcmp((const char *)nodes_old.at(set_old->at(i))->content, ",") != 0
      && strcmp((const char *)nodes_old.at(set_old->at(i))->name, "operator") != 0)
      set_old_text.push_back(set_old->at(i));

  node_set set_new_text(nodes_new);

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(set_new->at(i))) && !is_white_space(nodes_new.at(set_new->at(i)))
      && strcmp((const char *)nodes_new.at(set_new->at(i))->content, "(") != 0
      && strcmp((const char *)nodes_new.at(set_new->at(i))->content, ")") != 0
      && strcmp((const char *)nodes_new.at(set_new->at(i))->content, ";") != 0
      && strcmp((const char *)nodes_new.at(set_new->at(i))->content, ",") != 0
      && strcmp((const char *)nodes_new.at(set_new->at(i))->name, "operator") != 0)
      set_new_text.push_back(set_new->at(i));

  text_old_length = set_old_text.size();
  text_new_length = set_new_text.size();

  ses.compute((const void *)&set_old_text, set_old_text.size(), (const void *)&set_new_text, set_new_text.size());

}

int srcdiff_measure::compute_similarity() {

  int text_old_length;
  int text_new_length;

  return compute_similarity(text_old_length, text_new_length);

}

int srcdiff_measure::compute_similarity(int & text_old_length, int & text_new_length) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || (node_compare(nodes_old.at(set_old->at(0)), nodes_new.at(set_new->at(0))) != 0
        && !is_interchangeable_match((const char *)nodes_old.at(set_old->at(0))->name, (const char *)nodes_new.at(set_new->at(0))->name)
        && (strcmp((const char *)nodes_old.at(set_old->at(0))->name, "block") != 0
            || strcmp((const char *)nodes_new.at(set_new->at(0))->name, "block") != 0))) {

    return MAX_INT;

  }

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);

  compute_ses(ses, text_old_length, text_new_length);

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

void srcdiff_measure::compute_measures(int & similarity, int & difference, int & text_old_length, int & text_new_length) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || (node_compare(nodes_old.at(set_old->at(0)), nodes_new.at(set_new->at(0))) != 0
        && !is_interchangeable_match((const char *)nodes_old.at(set_old->at(0))->name, (const char *)nodes_new.at(set_new->at(0))->name)
        && (strcmp((const char *)nodes_old.at(set_old->at(0))->name, "block") != 0
            || strcmp((const char *)nodes_new.at(set_new->at(0))->name, "block") != 0))) {

    similarity = MAX_INT;
    difference = MAX_INT;

    return;

  }

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);

  compute_ses_important_text(ses, text_old_length, text_new_length);

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

static bool is_significant(xNodePtr node, const void * context) {

  return !is_text(node) && strcmp(node->name, "operator") != 0 
      && strcmp(node->name, "literal") != 0 && strcmp(node->name, "modifier") != 0;

}

void srcdiff_measure::compute_syntax_measures(int & similarity, int & difference, int & children_old_length, int & children_new_length) {

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || (node_compare(nodes_old.at(set_old->at(0)), nodes_new.at(set_new->at(0))) != 0
        && !is_interchangeable_match((const char *)nodes_old.at(set_old->at(0))->name, (const char *)nodes_new.at(set_new->at(0))->name)
        && (strcmp((const char *)nodes_old.at(set_old->at(0))->name, "block") != 0
            || strcmp((const char *)nodes_new.at(set_new->at(0))->name, "block") != 0))) {

    similarity = 0;
    difference = MAX_INT;

    return;

  }

  ShortestEditScript ses(node_set_syntax_compare, node_set_index, &dnodes);

  // collect subset of nodes
  node_sets next_node_sets_old(nodes_old, set_old->at(1), set_old->back(), is_significant);
  node_sets next_node_sets_new(nodes_new, set_new->at(1), set_new->back(), is_significant);
  children_old_length = next_node_sets_old.size();
  children_new_length = next_node_sets_new.size();
  int distance = ses.compute((const void *)&next_node_sets_old, children_old_length, (const void *)&next_node_sets_new, children_new_length);

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
