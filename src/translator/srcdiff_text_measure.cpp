#include <srcdiff_text_measure.hpp>


#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>
#include <cassert>

srcdiff_text_measure::srcdiff_text_measure(const node_set & set_original, const node_set & set_modified) 
  : srcdiff_measure(set_original, set_modified) {}

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

  a_similarity = std::min(delete_similarity, insert_similarity);

  if(a_similarity <= 0)
    a_similarity = 0;

}
