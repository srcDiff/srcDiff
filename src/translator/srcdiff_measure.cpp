#include <srcdiff_measure.hpp>

#include <algorithm>
#include <cassert>

srcdiff_measure::srcdiff_measure(const node_set & set_original,
                                 const node_set & set_modified) 
  : set_original(set_original),
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

  node_set set_original_text(set_original.nodes());

  for(unsigned int i = 0; i < olength; ++i)
    if(set_original.nodes().at(set_original.at(i))->is_text() && !set_original.nodes().at(set_original.at(i))->is_white_space())
      set_original_text.push_back(set_original.at(i));

  node_set set_modified_text(set_modified.nodes());

  for(unsigned int i = 0; i < nlength; ++i)
    if(set_modified.nodes().at(set_modified.at(i))->is_text() && !set_modified.nodes().at(set_modified.at(i))->is_white_space())
      set_modified_text.push_back(set_modified.at(i));

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

  ses.compute((const void *)&set_original_text, set_original_text.size(), (const void *)&set_modified_text, set_modified_text.size());

}

void srcdiff_measure::compute_ses_important_text(class shortest_edit_script & ses) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  node_set set_original_text(set_original.nodes());

  for(unsigned int i = 0; i < olength; ++i) {

    if(set_original.nodes().at(set_original.at(i))->name == "operator"
      || set_original.nodes().at(set_original.at(i))->name == "modifier") {

      if(set_original.nodes().at(set_original.at(i))->extra & 0x1) continue;

      if(set_original.nodes().at(set_original.at(i))->parent && (*set_original.nodes().at(set_original.at(i))->parent)->name != "name") continue;

      if((set_original.at(i) + 1) < set_original.nodes().size() && set_original.nodes().at(set_original.at(i) + 1)->is_text()
        && (*set_original.nodes().at(set_original.at(i) + 1)->content == "::")) continue;

      while(set_original.nodes().at(set_original.at(i))->type != XML_READER_TYPE_END_ELEMENT)
        ++i;

    }

    if(set_original.nodes().at(set_original.at(i))->is_text() && !set_original.nodes().at(set_original.at(i))->is_white_space()
      && set_original.nodes().at(set_original.at(i))->content
      && ((set_original.nodes().at(set_original.at(i))->parent &&
          (*set_original.nodes().at(set_original.at(i))->parent)->name == "operator")
      ||   (*set_original.nodes().at(set_original.at(i))->content != "("
        && *set_original.nodes().at(set_original.at(i))->content != ")"
        && *set_original.nodes().at(set_original.at(i))->content != "{"
        && *set_original.nodes().at(set_original.at(i))->content != "}"
        && *set_original.nodes().at(set_original.at(i))->content != ";"
        && *set_original.nodes().at(set_original.at(i))->content != ",")))
      set_original_text.push_back(set_original.at(i));

  }

  node_set set_modified_text(set_modified.nodes());

  for(unsigned int i = 0; i < nlength; ++i) {

    if(set_modified.nodes().at(set_modified.at(i))->name == "operator"
      || set_modified.nodes().at(set_modified.at(i))->name == "modifier") {

      if(set_modified.nodes().at(set_modified.at(i))->extra & 0x1) continue;

      if(set_modified.nodes().at(set_modified.at(i))->parent && (*set_modified.nodes().at(set_modified.at(i))->parent)->name != "name") continue;

      if((set_modified.at(i) + 1) < set_modified.nodes().size() && set_modified.nodes().at(set_modified.at(i) + 1)->is_text()
        && (*set_modified.nodes().at(set_modified.at(i) + 1)->content == "::")) continue;

      while(set_modified.nodes().at(set_modified.at(i))->type != XML_READER_TYPE_END_ELEMENT)
        ++i;

    }

    if(set_modified.nodes().at(set_modified.at(i))->is_text() && !set_modified.nodes().at(set_modified.at(i))->is_white_space()
      && set_modified.nodes().at(set_modified.at(i))->content
      && ((set_modified.nodes().at(set_modified.at(i))->parent
        && (*set_modified.nodes().at(set_modified.at(i))->parent)->name == "operator")
      ||  (*set_modified.nodes().at(set_modified.at(i))->content != "("
        && *set_modified.nodes().at(set_modified.at(i))->content != ")"
        && *set_modified.nodes().at(set_modified.at(i))->content != "{"
        && *set_modified.nodes().at(set_modified.at(i))->content != "}"
        && *set_modified.nodes().at(set_modified.at(i))->content != ";"
        && *set_modified.nodes().at(set_modified.at(i))->content != ",")))
      set_modified_text.push_back(set_modified.at(i));

  }

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

  ses.compute((const void *)&set_original_text, set_original_text.size(), (const void *)&set_modified_text, set_modified_text.size());

}
