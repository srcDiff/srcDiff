#include <srcdiff_comment.hpp>

#include <srcdiff_common.hpp>
#include <srcdiff_compare.hpp>
#include <shortest_edit_script.hpp>

srcdiff_comment::srcdiff_comment(srcdiff_output & out, const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified) 
  : srcdiff_diff(out, construct_list_original, construct_list_modified) {}

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void srcdiff_comment::output() {

  shortest_edit_script_t ses(srcdiff_compare::construct_compare, srcdiff_compare::construct_list_index, nullptr);

  int distance = ses.compute((const void *)&construct_list_original, construct_list_original.size(), (const void *)&construct_list_modified, construct_list_modified.size());

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

      diff_end_original = construct_list_original.at(edits->offset_sequence_one - 1).end_position() + 1;
      diff_end_modified = construct_list_modified.at(edits->offset_sequence_two - 1).end_position() + 1;

    } else if(edits->operation == SES_INSERT && edits->offset_sequence_one != 0 && last_diff_original <= edits->offset_sequence_one)  {

      diff_end_original = construct_list_original.at(edits->offset_sequence_one - 1).end_position() + 1;
      diff_end_modified = construct_list_modified.at(edits->offset_sequence_two - 1).end_position() + 1;

    }

    // output area in common
    srcdiff_common::output_common(out, diff_end_original, diff_end_modified);

    // detect and change
    edit_t * edit_next = edits->next;
    if(is_change(edits)) {

      // TODO: simplify unless plan to handle many to many different // 1-1
      if(edits->length == edit_next->length && edits->length == 1
         && (construct_list_original.at(edits->offset_sequence_one).size() > 1
             || construct_list_original.at(edits->offset_sequence_one).size() > 1)) {

        output_change_whitespace(construct_list_original.at(edits->offset_sequence_one).end_position() + 1, construct_list_modified.at(edit_next->offset_sequence_two).end_position() + 1);

      } else {

        // many to many
        output_change_whitespace(construct_list_original.at(edits->offset_sequence_one + edits->length - 1).end_position() + 1,
         construct_list_modified.at(edit_next->offset_sequence_two + edit_next->length - 1).end_position() + 1);

      }

      // update for common
      last_diff_original = edits->offset_sequence_one + edits->length;
      last_diff_modified = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case SES_INSERT:

        output_pure(0, construct_list_modified.at(edits->offset_sequence_two + edits->length - 1).end_position() + 1);

        // update for common
        last_diff_original = edits->offset_sequence_one;
        last_diff_modified = edits->offset_sequence_two + edits->length;

        break;

      case SES_DELETE:

        output_pure(construct_list_original.at(edits->offset_sequence_one + edits->length - 1).end_position() + 1, 0);

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
  if(last_diff_original < (signed)construct_list_original.size()) {

    diff_end_original = construct_list_original.back().end_position() + 1;
    diff_end_modified = construct_list_modified.back().end_position() + 1;

  }

  // output area in common
  srcdiff_common::output_common(out, diff_end_original, diff_end_modified);

}



