#include <srcdiff_change.hpp>

#include <srcdiff_whitespace.hpp>
#include <srcdiff_move.hpp>
#include <shortest_edit_script.h>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

#include <cstring>
#include <string>

const boost::optional<std::string> srcdiff_change::change("change");

srcdiff_change::srcdiff_change(const srcdiff_output & out, unsigned int end_original, unsigned int end_modified)
: srcdiff_output(out), end_original(end_original), end_modified(end_modified) {}

/*

  Adds whitespace to a change. Then outputs the change.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after is not a newline,
  it is included and the following nodes are included if they have a new line.

*/
void srcdiff_change::output_whitespace() {

  srcdiff_whitespace whitespace(*this);
  whitespace.output_prefix();

}

/*

  Outputs a syntactical diff. Beginning whitespace is narrowed and all
  whitespace even if it could be matches is treated as different.

*/
void srcdiff_change::output() {

  unsigned int begin_original = rbuf_original->last_output;
  unsigned int begin_modified = rbuf_modified->last_output;

  if(end_original > begin_original && end_modified > begin_modified) {

    // set attribute to change
    diff_type->value = change;
    diff_original_start->properties.push_back(*diff_type.get());
    diff_modified_start->properties.push_back(*diff_type.get());

  }

  if(end_original > begin_original) {


    for(unsigned int i = begin_original; i < end_original; ++i) {

      if(rbuf_original->nodes.at(i)->move) {

        srcdiff_move move(*this, i, SESDELETE);
        move.output();

        continue;

      }

      // output diff tag begin
      output_node(diff_original_start, SESDELETE);

      output_node(rbuf_original->nodes.at(i), SESDELETE);

      // output diff tag begin
      output_node(diff_original_end, SESDELETE);

    }

    // output diff tag begin
    output_node(diff_original_end, SESDELETE);

    rbuf_original->last_output = end_original;

  }

  if(end_modified > begin_modified) {

    for(unsigned int i = begin_modified; i < end_modified; ++i) {

      if(rbuf_modified->nodes.at(i)->move) {

        srcdiff_move move(*this, i, SESINSERT);
        move.output();

        continue;

      }

      // output diff tag
      output_node(diff_modified_start, SESINSERT);

      output_node(rbuf_modified->nodes.at(i), SESINSERT);

    // output diff tag begin
    output_node(diff_modified_end, SESINSERT);


    }

    // output diff tag begin
    output_node(diff_modified_end, SESINSERT);

    rbuf_modified->last_output = end_modified;

  }

  diff_original_start->properties.clear();
  diff_modified_start->properties.clear();

}
