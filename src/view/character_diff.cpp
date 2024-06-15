// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file character_diff.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <character_diff.hpp>

#include <view.hpp>

int char_compare(const void * c1, const void * c2, const void * context [[maybe_unused]]) {

  char ch1 = *(char *)c1;
  char ch2 = *(char *)c2;
  return ch1 != ch2;

}

const void * char_index(int index, const void * s, const void * context [[maybe_unused]]) {
  const std::string & str = *(const std::string *)s;
  return &str[index];
}

character_diff::character_diff(const versioned_string & str)
    : ses(char_compare, char_index, nullptr),
      str(str) {}

void character_diff::compute() {
    ses.compute(&str.original(), str.original().size(), &str.modified(), str.modified().size());
}

void character_diff::output(view_t & view, const std::string & type) {

    int difference = 0;
    int num_consecutive_edits = 0;
    for(const edit_t * edits = ses.script(); edits; edits = edits->next) {

      num_consecutive_edits += 1;
      difference += edits->length;

    }

    int min_size = std::min(str.original().size(), str.modified().size());
    bool is_diff_name = type == "name" && 4 * difference < min_size;
    bool is_diff_operator = type == "operator" && difference <= min_size;

    if(is_diff_name || is_diff_operator || num_consecutive_edits == 1) {

        std::size_t last_diff_original = 0;
        std::size_t last_diff_modified = 0;
        for(const edit_t * edits = ses.script(); edits; edits = edits->next) {

        if(edits->operation == SES_DELETE 
           && last_diff_original < edits->offset_sequence_one) {
            view.output_characters(str.original().substr(last_diff_original, edits->offset_sequence_one),
                                 view_t::COMMON);
        } else if(edits->operation == SES_INSERT && last_diff_modified < edits->offset_sequence_two) {
            view.output_characters(str.original().substr(last_diff_modified, edits->offset_sequence_two),
                                 view_t::COMMON);
        }

        last_diff_original = edits->offset_sequence_one + edits->length;
        last_diff_modified = edits->offset_sequence_two + edits->length;

        // handle pure delete or insert
        switch (edits->operation) {

        case SES_INSERT:
          view.output_characters(str.modified().substr(edits->offset_sequence_two, edits->length), view_t::INSERT);

          // update for common
          last_diff_original = edits->offset_sequence_one;
          last_diff_modified = edits->offset_sequence_two + edits->length;

          break;

        case SES_DELETE:
          view.output_characters(str.original().substr(edits->offset_sequence_one, edits->length), view_t::DELETE);

          // update for common
          last_diff_original = edits->offset_sequence_one + edits->length;
          last_diff_modified = edits->offset_sequence_two;

          break;
        }

      }

      if(last_diff_original < str.original().size()) {
        view.output_characters(str.original().substr(last_diff_original), view_t::COMMON);
      }


    } else {

      view.output_characters(str.original(), view_t::DELETE);
      view.output_characters(str.modified(), view_t::INSERT);

    }

}
