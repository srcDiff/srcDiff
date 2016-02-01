#include <srcdiff_text_measure.hpp>


#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>
#include <cassert>

srcdiff_text_measure::srcdiff_text_measure(const node_set & set_original, const node_set & set_modified) 
  : srcdiff_measure(set_original, set_modified),
    set_original_text(set_original.nodes()),
    set_modified_text(set_modified.nodes()) {}

void srcdiff_text_measure::collect_text(class shortest_edit_script & ses) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

  for(unsigned int i = 0; i < olength; ++i)
    if(set_original.nodes().at(set_original.at(i))->is_text() && !set_original.nodes().at(set_original.at(i))->is_white_space())
      set_original_text.push_back(set_original.at(i));

  for(unsigned int i = 0; i < nlength; ++i)
    if(set_modified.nodes().at(set_modified.at(i))->is_text() && !set_modified.nodes().at(set_modified.at(i))->is_white_space())
      set_modified_text.push_back(set_modified.at(i));

  original_len = set_original_text.size();
  modified_len = set_modified_text.size();

}

void srcdiff_text_measure::collect_important_text(class shortest_edit_script & ses) {

  unsigned int olength = set_original.size();
  unsigned int nlength = set_modified.size();

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

}

void srcdiff_text_measure::compute() {

  if(computed) return;

  computed = true;

  diff_nodes dnodes = { set_original.nodes(), set_modified.nodes() };

  if((xmlReaderTypes)set_original.nodes().at(set_original.at(0))->type != XML_READER_TYPE_ELEMENT
     || (xmlReaderTypes)set_modified.nodes().at(set_modified.at(0))->type != XML_READER_TYPE_ELEMENT
     || (srcdiff_compare::node_compare(set_original.nodes().at(set_original.at(0)), set_modified.nodes().at(set_modified.at(0))) != 0
        && !srcdiff_match::is_interchangeable_match(set_original.nodes().at(set_original.at(0))->name, set_original.nodes().at(set_original.at(0))->ns->href,
                                                    set_modified.nodes().at(set_modified.at(0))->name, set_modified.nodes().at(set_modified.at(0))->ns->href)
        && (set_original.nodes().at(set_original.at(0))->name != "block" || set_modified.nodes().at(set_modified.at(0))->name != "block"))) {

    a_similarity = MAX_INT;
    a_difference = MAX_INT;

    return;

  }

  class shortest_edit_script ses(srcdiff_compare::node_index_compare, srcdiff_compare::node_index, &dnodes);

  collect_important_text(ses);

  if(original_len < 1000 || modified_len < 1000) {

    ses.compute((const void *)&set_original_text, original_len, (const void *)&set_modified_text, modified_len);

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

    a_similarity = std::min(delete_similarity, insert_similarity);

    if(a_similarity <= 0)
      a_similarity = 0;

  } else {

    for(int i = 0; i < original_len && i < modified_len; ++i)
      if(set_original_text.at(i) == set_modified_text.at(i))
        ++a_similarity;
      else
        ++a_difference;

  }

}
