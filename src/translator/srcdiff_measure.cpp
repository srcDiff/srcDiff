#include <srcdiff_measure.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_constants.hpp>

#include <cstring>

srcdiff_measure::srcdiff_measure(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_set & set_original, const node_set & set_modified) 
  : nodes_original(nodes_original), nodes_modified(nodes_modified), set_original(set_original), set_modified(set_modified) {}

int srcdiff_measure::similarity() const { return a_similarity; }

int srcdiff_measure::difference() const { return a_difference; }

int srcdiff_measure::original_length() const { return original_len; }

int srcdiff_measure::modified_length() const { return modified_len; }

void srcdiff_measure::compute_ses(class shortest_edit_script & ses) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  node_set set_original_text(nodes_original);

  for(unsigned int i = 0; i < olength; ++i)
    if(nodes_original.at(set_original.at(i))->is_text() && !nodes_original.at(set_original.at(i))->is_white_space())
      set_original_text.push_back(set_original.at(i));

  node_set set_modified_text(nodes_modified);

  for(unsigned int i = 0; i < nlength; ++i)
    if(nodes_modified.at(set_modified.at(i))->is_text() && !nodes_modified.at(set_modified.at(i))->is_white_space())
      set_modified_text.push_back(set_modified.at(i));

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

  ses.compute((const void *)&set_original_text, set_original_text.size(), (const void *)&set_modified_text, set_modified_text.size());

}

void srcdiff_measure::compute_ses_important_text(class shortest_edit_script & ses) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

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
      && ((nodes_original.at(set_original.at(i))->parent &&
          (*nodes_original.at(set_original.at(i))->parent)->name == "operator")
      ||   (*nodes_original.at(set_original.at(i))->content != "("
        && *nodes_original.at(set_original.at(i))->content != ")"
        && *nodes_original.at(set_original.at(i))->content != "{"
        && *nodes_original.at(set_original.at(i))->content != "}"
        && *nodes_original.at(set_original.at(i))->content != ";"
        && *nodes_original.at(set_original.at(i))->content != ",")))
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
      && ((nodes_modified.at(set_modified.at(i))->parent
        && (*nodes_modified.at(set_modified.at(i))->parent)->name == "operator")
      ||  (*nodes_modified.at(set_modified.at(i))->content != "("
        && *nodes_modified.at(set_modified.at(i))->content != ")"
        && *nodes_modified.at(set_modified.at(i))->content != "{"
        && *nodes_modified.at(set_modified.at(i))->content != "}"
        && *nodes_modified.at(set_modified.at(i))->content != ";"
        && *nodes_modified.at(set_modified.at(i))->content != ",")))
      set_modified_text.push_back(set_modified.at(i));

  }

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

  ses.compute((const void *)&set_original_text, set_original_text.size(), (const void *)&set_modified_text, set_modified_text.size());

}

void srcdiff_measure::compute_similarity() {

  diff_nodes dnodes = { nodes_original, nodes_modified };

  if((xmlReaderTypes)nodes_original.at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_modified.at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(nodes_original.at(set_original.at(0)), nodes_modified.at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(nodes_original.at(set_original.at(0))->name, nodes_original.at(set_original.at(0))->ns->href,
                                                    nodes_modified.at(set_modified.at(0))->name, nodes_modified.at(set_modified.at(0))->ns->href)
        && (nodes_original.at(set_original.at(0))->name != "block" || nodes_modified.at(set_modified.at(0))->name != "block"))) {

    a_similarity = MAX_INT;

  }

  class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_index, &dnodes);

  compute_ses(ses);

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

  delete_similarity = original_len - delete_similarity;
  insert_similarity = modified_len - insert_similarity;

  a_similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(a_similarity <= 0)
    a_similarity = 0;

}

void srcdiff_measure::compute_measures() {

  diff_nodes dnodes = { nodes_original, nodes_modified };

  if((xmlReaderTypes)nodes_original.at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_modified.at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(nodes_original.at(set_original.at(0)), nodes_modified.at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(nodes_original.at(set_original.at(0))->name, nodes_original.at(set_original.at(0))->ns->href,
                                                    nodes_modified.at(set_modified.at(0))->name, nodes_modified.at(set_modified.at(0))->ns->href)
        && (nodes_original.at(set_original.at(0))->name != "block" || nodes_modified.at(set_modified.at(0))->name != "block"))) {

    a_similarity = MAX_INT;
    a_difference = MAX_INT;

    return;

  }

  class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_index, &dnodes);

  compute_ses_important_text(ses);

  edit * edits = ses.get_script();

  a_similarity = 0, a_difference = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    a_difference += edits->length;

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = original_len - delete_similarity;
  insert_similarity = modified_len - insert_similarity;

  a_similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(a_similarity <= 0)
    a_similarity = 0;

}

static bool is_significant(int & node_pos, const srcml_nodes & nodes, const void * context) {

  const std::shared_ptr<srcml_node> & node = nodes[node_pos];

  if(node->name == "argument_list") {

    int pos = node_pos + 1;
    while(pos < nodes.size() && (nodes[pos]->type == XML_READER_TYPE_ELEMENT || nodes[pos]->name != "argument_list")) {

      if(!nodes[pos]->is_text()) return true;

      ++pos;

    }

    node_pos = pos;

    return false;

  }

  return !node->is_text() && node->name != "operator"
      && node->name != "literal" && node->name != "modifier";

}

void srcdiff_measure::compute_syntax_measures() {

  diff_nodes dnodes = { nodes_original, nodes_modified };

  if((xmlReaderTypes)nodes_original.at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)nodes_modified.at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(nodes_original.at(set_original.at(0)), nodes_modified.at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(nodes_original.at(set_original.at(0))->name, nodes_original.at(set_original.at(0))->ns->href,
                                                    nodes_modified.at(set_modified.at(0))->name, nodes_modified.at(set_modified.at(0))->ns->href)
        && (nodes_original.at(set_original.at(0))->name != "block" || nodes_modified.at(set_modified.at(0))->name != "block"))) {

    a_similarity = 0;
    a_difference = MAX_INT;

    return;

  }

  class shortest_edit_script ses(srcdiff_compare::node_set_syntax_compare, srcdiff_compare::node_set_index, &dnodes);

  // collect subset of nodes
  node_sets next_node_sets_original = set_original.size() > 1 ? node_sets(nodes_original, set_original.at(1), set_original.back(), is_significant) : node_sets(nodes_original);
  node_sets next_node_sets_modified = set_modified.size() > 1 ? node_sets(nodes_modified, set_modified.at(1), set_modified.back(), is_significant) : node_sets(nodes_modified);
  original_len = next_node_sets_original.size();
  modified_len = next_node_sets_modified.size();
  int distance = ses.compute((const void *)&next_node_sets_original, original_len, (const void *)&next_node_sets_modified, modified_len);

  edit * edits = ses.get_script();

  a_similarity = 0, a_difference = 0;

  int delete_similarity = 0;
  int insert_similarity = 0;
  for(; edits; edits = edits->next) {

    a_difference += edits->length;

    switch(edits->operation) {

      case SES_DELETE :

        delete_similarity += edits->length;
        break;

      case SES_INSERT :

        insert_similarity += edits->length;
        break;

      }

  }

  delete_similarity = original_len - delete_similarity;
  insert_similarity = modified_len - insert_similarity;

  a_similarity = delete_similarity < insert_similarity ? delete_similarity : insert_similarity;

  if(a_similarity <= 0)
    a_similarity = 0;

}
