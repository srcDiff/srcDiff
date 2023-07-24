#include <srcdiff_comment.hpp>

#include <srcdiff_common.hpp>
#include <srcdiff_compare.hpp>
#include <shortest_edit_script.hpp>

srcdiff_comment::srcdiff_comment(srcdiff_output & out, const element_list & element_list_original, const element_list & element_list_modified) 
  : srcdiff_diff(out, element_list_original, element_list_modified) {}

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void srcdiff_comment::output() {

  diff_nodes dnodes = { out.nodes_original(), out.nodes_modified() };
  shortest_edit_script_t ses(srcdiff_compare::element_syntax_compare, srcdiff_compare::element_index, &dnodes);

  int distance = ses.compute((const void *)&element_list_original, element_list_original.size(), (const void *)&element_list_modified, element_list_modified.size());

  edit_t * edit_script = ses.script();

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_original = 0;
  int last_diff_modified = 0;
  int diff_end_original = out.last_output_original();
  int diff_end_modified = out.last_output_modified();

  edit_t * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_original = out.last_output_original();
    diff_end_modified = out.last_output_modified();
    if(edits->operation == SES_DELETE && last_diff_original < edits->offset_sequence_one) {

      diff_end_original = element_list_original.at(edits->offset_sequence_one - 1).back() + 1;
      diff_end_modified = element_list_modified.at(edits->offset_sequence_two - 1).back() + 1;

    } else if(edits->operation == SES_INSERT && edits->offset_sequence_one != 0 && last_diff_original <= edits->offset_sequence_one)  {

      diff_end_original = element_list_original.at(edits->offset_sequence_one - 1).back() + 1;
      diff_end_modified = element_list_modified.at(edits->offset_sequence_two - 1).back() + 1;

    }

    // output area in common
    output_common(diff_end_original, diff_end_modified);

    // detect and change
    edit_t * edit_next = edits->next;
    if(is_change(edits)) {

      // TODO: simplify unless plan to handle many to many different // 1-1
      if(edits->length == edit_next->length && edits->length == 1
         && (element_list_original.at(edits->offset_sequence_one).size() > 1
             || element_list_original.at(edits->offset_sequence_one).size() > 1)) {

        output_change_whitespace(element_list_original.at(edits->offset_sequence_one).back() + 1, element_list_modified.at(edit_next->offset_sequence_two).back() + 1);

      } else {

        // many to many
        output_change_whitespace(element_list_original.at(edits->offset_sequence_one + edits->length - 1).back() + 1,
         element_list_modified.at(edit_next->offset_sequence_two + edit_next->length - 1).back() + 1);

      }

      // update for common
      last_diff_original = edits->offset_sequence_one + edits->length;
      last_diff_modified = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case SES_INSERT:

        output_pure(0, element_list_modified.at(edits->offset_sequence_two + edits->length - 1).back() + 1);

        // update for common
        last_diff_original = edits->offset_sequence_one;
        last_diff_modified = edits->offset_sequence_two + edits->length;

        break;

      case SES_DELETE:

        output_pure(element_list_original.at(edits->offset_sequence_one + edits->length - 1).back() + 1, 0);

        // update for common
        last_diff_original = edits->offset_sequence_one + edits->length;
        last_diff_modified = edits->offset_sequence_two;

        break;
      }

    }
  }

  // determine ending position to output
  diff_end_original = out.last_output_original();
  diff_end_modified = out.last_output_modified();
  if(last_diff_original < (signed)element_list_original.size()) {

    diff_end_original = element_list_original.back().back() + 1;
    diff_end_modified = element_list_modified.back().back() + 1;

  }

  // output area in common
  output_common(diff_end_original, diff_end_modified);

}



