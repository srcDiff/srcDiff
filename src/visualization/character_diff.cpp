#include <character_diff.hpp>

#include <bash_view.hpp>

static int compare_char(const void * item_one, const void * item_two, const void * context) {

  const char * char1 = (const char *)item_one;
  const char * char2 = (const char *)item_two;

  return (*char1) != (*char2);

}

static const void * access_char(int index, const void * structure, const void * context) {

  const char * str = (const char *)structure;
  return &str[index];

}

character_diff::character_diff(const std::string & original, const std::string & modified)
    : ses(compare_char, access_char, nullptr),
      original_str(original), modified_str(modified) {}

void character_diff::compute() {

    ses.compute<std::string>(original_str, modified_str, false);

}

void character_diff::output(bash_view & view, const std::string & type) {

    int difference = 0;
    for(const edit * edits = ses.get_script(); edits; edits = edits->next)
      difference += edits->length;

    int min_size = std::min(original_str.size(), modified_str.size());
    if(  (type == "name" && 5 * difference < min_size)
      ||  (type == "operator" && difference <= min_size)) {

      int last_diff_original = 0;
      for(const edit * edits = ses.get_script(); edits; edits = edits->next) {

        if(edits->operation == SES_DELETE 
           && last_diff_original < edits->offset_sequence_one)
          view.output_characters(original_str.substr(last_diff_original, 
                                            edits->offset_sequence_one),
                            bash_view::COMMON);
        else if(edits->operation == SES_INSERT && edits->offset_sequence_one != 0
                && last_diff_original <= edits->offset_sequence_one)
          view.output_characters(original_str.substr(last_diff_original, 
                                            edits->offset_sequence_one),
                            bash_view::COMMON);

        // handle pure delete or insert
        switch (edits->operation) {

        case SES_INSERT:

          view.output_characters(modified_str.substr(edits->offset_sequence_two, edits->length), bash_view::INSERT);

          // update for common
          last_diff_original = edits->offset_sequence_one;

          break;

        case SES_DELETE:

          view.output_characters(original_str.substr(edits->offset_sequence_one, edits->length), bash_view::DELETE);

          // update for common
          last_diff_original = edits->offset_sequence_one + edits->length;

          break;
        }

      }

      if(last_diff_original < (signed)original_str.size())
        view.output_characters(original_str.substr(last_diff_original), bash_view::COMMON);


    } else {

      view.output_characters(original_str, bash_view::DELETE);
      view.output_characters(modified_str, bash_view::INSERT);

    }

}
