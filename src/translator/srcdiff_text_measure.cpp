#include <srcdiff_text_measure.hpp>


#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>
#include <srcdiff_match.hpp>
#include <srcdiff_diff.hpp>

#include <algorithm>
#include <cassert>

srcdiff_text_measure::srcdiff_text_measure(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_set & set_original, const node_set & set_modified) 
  : srcdiff_measure(nodes_original, nodes_modified, set_original, set_modified) {}

void srcdiff_text_measure::compute() {

  if(computed) return;

  computed = true;

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

  a_similarity = std::min(delete_similarity, insert_similarity);

  if(a_similarity <= 0)
    a_similarity = 0;

}
