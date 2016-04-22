#include <character_diff.hpp>

#include <view.hpp>

static int compare_char(const void * item_one, const void * item_two, const void * context) {

  const char * char1 = (const char *)item_one;
  const char * char2 = (const char *)item_two;

  return (*char1) != (*char2);

}

static const void * access_char(int index, const void * structure, const void * context) {

  const char * str = (const char *)structure;
  return &str[index];

}

character_diff::character_diff(const versioned_string & str)
    : ses(compare_char, access_char, nullptr),
      str(str) {}

void character_diff::compute() {

    ses.compute<std::string>(str.original(), str.modified(), false);

}

void character_diff::output(view_t & view, const std::string & type) {

    int difference = 0;
    int num_consecutive_edits = 0;
    for(const edit * edits = ses.get_script(); edits; edits = edits->next) {

      num_consecutive_edits += 1;
      difference += edits->length;

    }

    int min_size = std::min(str.original().size(), str.modified().size());
    bool is_diff_name = type == "name" && 5 * difference < min_size;
    bool is_diff_operator = type == "operator" && difference <= min_size;
    // bool is_delete_consecutive = num_consecutive_edits == 1 
    //   && ses.get_script()->operation == SES_DELETE 
    //   && (ses.get_script()->offset_sequence_one == 0
    //     || (ses.get_script()->offset_sequence_one + ses.get_script()->length == str.original().size()));
    // bool is_insert_consecutive = num_consecutive_edits == 1 
    //   && ses.get_script()->operation == SES_INSERT 
    //   && (ses.get_script()->offset_sequence_two == 0
    //     || (ses.get_script()->offset_sequence_two + ses.get_script()->length == str.modified().size()));

    if(is_diff_name || is_diff_operator || num_consecutive_edits == 1) {

      int last_diff_original = 0;
      for(const edit * edits = ses.get_script(); edits; edits = edits->next) {

        if(edits->operation == SES_DELETE 
           && last_diff_original < edits->offset_sequence_one)
          view.output_characters(str.original().substr(last_diff_original, 
                                            edits->offset_sequence_one),
                            view_t::COMMON);
        else if(edits->operation == SES_INSERT && edits->offset_sequence_one != 0
                && last_diff_original <= edits->offset_sequence_one)
          view.output_characters(str.original().substr(last_diff_original, 
                                            edits->offset_sequence_one),
                            view_t::COMMON);

        // handle pure delete or insert
        switch (edits->operation) {

        case SES_INSERT:

          view.output_characters(str.modified().substr(edits->offset_sequence_two, edits->length), view_t::INSERT);

          // update for common
          last_diff_original = edits->offset_sequence_one;

          break;

        case SES_DELETE:

          view.output_characters(str.original().substr(edits->offset_sequence_one, edits->length), view_t::DELETE);

          // update for common
          last_diff_original = edits->offset_sequence_one + edits->length;

          break;
        }

      }

      if(last_diff_original < (signed)str.original().size())
        view.output_characters(str.original().substr(last_diff_original), view_t::COMMON);


    } else {

      view.output_characters(str.original(), view_t::DELETE);
      view.output_characters(str.modified(), view_t::INSERT);

    }

}
