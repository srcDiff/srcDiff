#include "srcDiffMeasure.hpp"

#include "srcDiffTypes.hpp"
#include "srcDiffUtility.hpp"
#include "shortest_edit_script.h"
#include "ShortestEditScript.hpp"

int compute_similarity(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return MAX_INT;

  }

  std::vector<int> node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  std::vector<int> node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);
  ses.compute(node_set_old_text.size(), (const void *)&node_set_old_text, node_set_new_text.size(), (const void *)&node_set_new_text);

  edit * edits = ses.get_script();
  unsigned int similarity = 0;
  int last_offset = 0;
  for(; edits; edits = edits->next) {
    /*
    if(edits->operation == SESDELETE)
      for(int i = last_offset; i < edits->offset_sequence_one; ++i)
        ++similarity;
    else if(edits->operation == SESINSERT)
      for(int i = last_offset; i < edits->offset_sequence_one + 1; ++i)
        ++similarity;

    if(is_change(edits)) {


      last_offset = edits->offset_sequence_one + edits->length;

      edits = edits->next;
      continue;
    }
    */
    switch(edits->operation) {

      /*
    case SESINSERT :

      last_offset = edits->offset_sequence_one + 1;

      break;
      */
    case SESDELETE :

      //last_offset = edits->offset_sequence_one + edits->length;
      similarity += edits->length;
      break;

      }

  }

  //for(int i = last_offset; i < node_set_old_text.size(); ++i)
  //++similarity;

  similarity = node_set_old_text.size() - similarity;

  if(similarity <= 0)
    similarity = 0;

  return similarity;

}

int compute_difference(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                       std::vector<int> * node_set_new) {

  unsigned int olength = node_set_old->size();
  unsigned int nlength = node_set_new->size();

  diff_nodes dnodes = { nodes_old, nodes_new };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old.nodes.at(node_set_old->at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_new.at(node_set_new->at(0))->name);

  if((xmlReaderTypes)nodes_old.at(node_set_old->at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_new.at(node_set_new->at(0))->type != XML_READER_TYPE_ELEMENT
     || node_compare(nodes_old.at(node_set_old->at(0)), nodes_new.at(node_set_new->at(0))) != 0) {

    return MAX_INT;

  }

  std::vector<int> node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  std::vector<int> node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  ShortestEditScript ses(node_index_compare, node_index, &dnodes);

  edit * edit_script;
  shortest_edit_script(node_set_old_text.size(), (void *)&node_set_old_text, node_set_new_text.size(),
                       (void *)&node_set_new_text, node_index_compare, node_index, &edit_script, &dnodes);

  edit * edits = edit_script;
  unsigned int similarity = 0;
  int last_offset;
  for(; edits; edits = edits->next) {

    if(0 && is_change(edits)) {

      similarity += edits->length > edits->next->length ? edits->length : edits->next->length;

      edits = edits->next;

    } else {

      similarity += edits->length;

    }


  }

  free_shortest_edit_script(edit_script);

  return similarity;

}

int compute_similarity_old(std::vector<xNodePtr> & nodes_old, std::vector<int> * node_set_old, std::vector<xNodePtr> & nodes_new,
                           std::vector<int> * node_set_new) {

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

  std::vector<int> node_set_old_text;

  for(unsigned int i = 0; i < olength; ++i)
    if(is_text(nodes_old.at(node_set_old->at(i))) && !is_white_space(nodes_old.at(node_set_old->at(i))))
      node_set_old_text.push_back(node_set_old->at(i));

  std::vector<int> node_set_new_text;

  for(unsigned int i = 0; i < nlength; ++i)
    if(is_text(nodes_new.at(node_set_new->at(i))) && !is_white_space(nodes_new.at(node_set_new->at(i))))
      node_set_new_text.push_back(node_set_new->at(i));

  edit * edit_script;
  shortest_edit_script(node_set_old_text.size(), (void *)&node_set_old_text, node_set_new_text.size(),
                       (void *)&node_set_new_text, node_index_compare, node_index, &edit_script, &dnodes);

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
