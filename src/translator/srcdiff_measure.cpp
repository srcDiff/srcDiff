#include <srcdiff_measure.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_constants.hpp>

#include <cstring>

srcdiff_measure::srcdiff_measure(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_set & set_original, const node_set & set_modified) 
  : nodes_original(nodes_original), nodes_modified(nodes_modified), set_original(set_original), set_modified(set_modified) {}

void srcdiff_measure::compute_ses(class shortest_edit_script & ses, int & text_original_length, int & text_modified_length) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original.nodes.at(set_original.at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_modified.at(set_modified.at(0))->name);

  node_set set_original_text(nodes_original);

  for(unsigned int i = 0; i < olength; ++i)
    if(nodes_original.at(set_original.at(i))->is_text() && !nodes_original.at(set_original.at(i))->is_white_space())
      set_original_text.push_back(set_original.at(i));

  node_set set_modified_text(nodes_modified);

  for(unsigned int i = 0; i < nlength; ++i)
    if(nodes_modified.at(set_modified.at(i))->is_text() && !nodes_modified.at(set_modified.at(i))->is_white_space())
      set_modified_text.push_back(set_modified.at(i));

  text_original_length = set_original_text.size();
  text_modified_length = set_modified_text.size();

  ses.compute((const void *)&set_original_text, set_original_text.size(), (const void *)&set_modified_text, set_modified_text.size());

}

void srcdiff_measure::compute_ses_important_text(class shortest_edit_script & ses, int & text_original_length, int & text_modified_length) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original.nodes.at(set_original.at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_modified.at(set_modified.at(0))->name);

  node_set set_original_text(nodes_original);

  for(unsigned int i = 0; i < olength; ++i) {

    if(nodes_original.at(set_original.at(i))->name == "operator"
      || nodes_original.at(set_original.at(i))->name == "modifier") {

      if(nodes_original.at(set_original.at(i))->extra & 0x1) continue;

      if(nodes_original.at(set_original.at(i))->parent && (*nodes_original.at(set_original.at(i))->parent)->name != "name") continue;

      if((set_original.at(i) + 1) < nodes_original.size() && nodes_original.at(set_original.at(i) + 1)->is_text()
        && (*nodes_original.at(set_original.at(i) + 1)->content == "::")) continue;

      while(nodes_original.at(set_original.at(i))->type != XML_READER_TYPE_END_ELEMENT)
        ++i;

    }

    if(nodes_original.at(set_original.at(i))->is_text() && !nodes_original.at(set_original.at(i))->is_white_space()
      && nodes_original.at(set_original.at(i))->content
      && *nodes_original.at(set_original.at(i))->content != "("
      && *nodes_original.at(set_original.at(i))->content != ")"
      && *nodes_original.at(set_original.at(i))->content != "{"
      && *nodes_original.at(set_original.at(i))->content != "}"
      && *nodes_original.at(set_original.at(i))->content != ";"
      && *nodes_original.at(set_original.at(i))->content != ",")
      set_original_text.push_back(set_original.at(i));

  }

  node_set set_modified_text(nodes_modified);

  for(unsigned int i = 0; i < nlength; ++i) {

    if(nodes_modified.at(set_modified.at(i))->name == "operator"
      || nodes_modified.at(set_modified.at(i))->name == "modifier") {

      if(nodes_modified.at(set_modified.at(i))->extra & 0x1) continue;

      if(nodes_modified.at(set_modified.at(i))->parent && (*nodes_modified.at(set_modified.at(i))->parent)->name != "name") continue;

      if((set_modified.at(i) + 1) < nodes_modified.size() && nodes_modified.at(set_modified.at(i) + 1)->is_text()
        && (*nodes_modified.at(set_modified.at(i) + 1)->content == "::")) continue;

      while(nodes_modified.at(set_modified.at(i))->type != XML_READER_TYPE_END_ELEMENT)
        ++i;

    }

    if(nodes_modified.at(set_modified.at(i))->is_text() && !nodes_modified.at(set_modified.at(i))->is_white_space()
      && nodes_modified.at(set_modified.at(i))->content
      && *nodes_modified.at(set_modified.at(i))->content != "("
      && *nodes_modified.at(set_modified.at(i))->content != ")"
      && *nodes_modified.at(set_modified.at(i))->content != "{"
      && *nodes_modified.at(set_modified.at(i))->content != "}"
      && *nodes_modified.at(set_modified.at(i))->content != ";"
      && *nodes_modified.at(set_modified.at(i))->content != ",")
      set_modified_text.push_back(set_modified.at(i));

  }

  text_original_length = set_original_text.size();
  text_modified_length = set_modified_text.size();

  ses.compute((const void *)&set_original_text, set_original_text.size(), (const void *)&set_modified_text, set_modified_text.size());

}

int srcdiff_measure::compute_similarity() {

  int text_original_length;
  int text_modified_length;

  return compute_similarity(text_original_length, text_modified_length);

}

int srcdiff_measure::compute_similarity(int & text_original_length, int & text_modified_length) {

  diff_nodes dnodes = { nodes_original, nodes_modified };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original.nodes.at(set_original.at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_modified.at(set_modified.at(0))->name);

  if((xmlReaderTypes)nodes_original.at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_modified.at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(nodes_original.at(set_original.at(0)), nodes_modified.at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(nodes_original.at(set_original.at(0))->name, nodes_original.at(set_original.at(0))->ns->href,
                                                    nodes_modified.at(set_modified.at(0))->name, nodes_modified.at(set_modified.at(0))->ns->href)
        && (nodes_original.at(set_original.at(0))->name != "block" || nodes_modified.at(set_modified.at(0))->name != "block"))) {

    return MAX_INT;

  }

  class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_index, &dnodes);

  compute_ses(ses, text_original_length, text_modified_length);

  edit * edits = ses.get_script();

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = text_original_length - delete_similarity;
  insert_similarity = text_modified_length - insert_similarity;

  int similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity <= 0)
    similarity = 0;

  return similarity;

}

void srcdiff_measure::compute_measures(int & similarity, int & difference, int & text_original_length, int & text_modified_length) {

  diff_nodes dnodes = { nodes_original, nodes_modified };

  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original.nodes.at(set_original.at(0))->name);
  //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, nodes_modified.at(set_modified.at(0))->name);

  if((xmlReaderTypes)nodes_original.at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_modified.at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(nodes_original.at(set_original.at(0)), nodes_modified.at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(nodes_original.at(set_original.at(0))->name, nodes_original.at(set_original.at(0))->ns->href,
                                                    nodes_modified.at(set_modified.at(0))->name, nodes_modified.at(set_modified.at(0))->ns->href)
        && (nodes_original.at(set_original.at(0))->name != "block" || nodes_modified.at(set_modified.at(0))->name != "block"))) {

    similarity = MAX_INT;
    difference = MAX_INT;

    return;

  }

  class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_index, &dnodes);

  compute_ses_important_text(ses, text_original_length, text_modified_length);

  edit * edits = ses.get_script();

  similarity = 0, difference = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    difference += edits->length;

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = text_original_length - delete_similarity;
  insert_similarity = text_modified_length - insert_similarity;

  similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity <= 0)
    similarity = 0;

}

static bool is_significant(size_t node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  if(node->name == "argument_list") {

    size_t pos = node_pos + 1;
    while(pos < nodes.size() && (nodes[pos]->type == XML_READER_TYPE_ELEMENT || nodes[pos]->name != "argument_list")) {

      if(!nodes[pos]->is_text()) return true;

      ++pos;

    }

    return false;

  }

  return !node->is_text() && node->name != "operator"
      && node->name != "literal" && node->name != "modifier";

}

void srcdiff_measure::compute_syntax_measures(int & similarity, int & difference, int & children_original_length, int & children_modified_length) {

  diff_nodes dnodes = { nodes_original, nodes_modified };

  if((xmlReaderTypes)nodes_original.at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_modified.at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(nodes_original.at(set_original.at(0)), nodes_modified.at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(nodes_original.at(set_original.at(0))->name, nodes_original.at(set_original.at(0))->ns->href,
                                                    nodes_modified.at(set_modified.at(0))->name, nodes_modified.at(set_modified.at(0))->ns->href)
        && (nodes_original.at(set_original.at(0))->name != "block" || nodes_modified.at(set_modified.at(0))->name != "block"))) {

    similarity = 0;
    difference = MAX_INT;

    return;

  }

  class shortest_edit_script ses(srcdiff_compare::node_set_syntax_compare, srcdiff_compare::node_set_index, &dnodes);

  // collect subset of nodes
  node_sets next_node_sets_original = set_original.size() > 1 ? node_sets(nodes_original, set_original.at(1), set_original.back(), is_significant) : node_sets(nodes_original);
  node_sets next_node_sets_modified = set_modified.size() > 1 ? node_sets(nodes_modified, set_modified.at(1), set_modified.back(), is_significant) : node_sets(nodes_modified);
  children_original_length = next_node_sets_original.size();
  children_modified_length = next_node_sets_modified.size();
  int distance = ses.compute((const void *)&next_node_sets_original, children_original_length, (const void *)&next_node_sets_modified, children_modified_length);

  edit * edits = ses.get_script();

  similarity = 0, difference = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    difference += edits->length;

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = children_original_length - delete_similarity;
  insert_similarity = children_modified_length - insert_similarity;

  similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(similarity <= 0)
    similarity = 0;

}
