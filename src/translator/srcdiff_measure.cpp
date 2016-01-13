#include <srcdiff_measure.hpp>

#include <algorithm>
#include <cassert>

srcdiff_measure::srcdiff_measure(const srcml_nodes & nodes_original,
                                 const srcml_nodes & nodes_modified,
                                 const node_set & set_original,
                                 const node_set & set_modified) 
  : nodes_original(nodes_original),
    nodes_modified(nodes_modified),
    set_original(set_original),
    set_modified(set_modified),
    computed(false),
    a_similarity(0),
    a_difference(0),
    original_len(0),
    modified_len(0) {}

int srcdiff_measure::similarity() const {

  assert(computed);

  return a_similarity;

}

int srcdiff_measure::difference() const {

  assert(computed);

  return a_difference;

}

int srcdiff_measure::original_length() const {

  assert(computed);

  return original_len;

}

int srcdiff_measure::modified_length() const {

  assert(computed);

  return modified_len;

}

int srcdiff_measure::max_length() const {

  assert(computed);

  return std::max(original_len, modified_len);

}

int srcdiff_measure::min_length() const {

  assert(computed);

  return std::min(original_len, modified_len);

}


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
