#include <srcdiff_change.hpp>

#include <srcdiff_whiteSpace.hpp>
#include <srcdiff_move.hpp>

#include <srcdiff_constants.hpp>
#include <shortest_edit_script.h>
#include <xmlrw.hpp>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

#include <string.h>
#include <string>

srcdiff_change::srcdiff_change(const srcdiff_output & out, unsigned int end_old, unsigned int end_new)
: srcdiff_output(out), end_old(end_old), end_new(end_new) {}

/*

  Adds whitespace to a change. Then outputs the change.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after is not a newline,
  it is included and the following nodes are included if they have a new line.

*/
void srcdiff_change::output_whitespace() {

  int oend = end_old;
  int nend = end_new;

  srcdiff_whitespace whitespace(*this);
  whitespace.output_prefix();

}

/*

  Outputs a syntactical diff. Beginning whitespace is narrowed and all
  whitespace even if it could be matches is treated as different.

*/
void srcdiff_change::output() {

  unsigned int begin_old = rbuf_old->last_output;
  unsigned int begin_new = rbuf_new->last_output;

  if(end_old > begin_old && end_new > begin_new) {

    // set attribute to change
    diff_type->value = change;
    diff_old_start->properties = diff_type.get();
    diff_new_start->properties = diff_type.get();

  }

  if(end_old > begin_old) {


    for(unsigned int i = begin_old; i < end_old; ++i) {

      if(rbuf_old->nodes.at(i)->move) {

        srcdiff_move move(*this, i, SESDELETE);
        move.output();

        continue;

      }

      // output diff tag begin
      output_node(diff_old_start.get(), SESDELETE);

      output_node(rbuf_old->nodes.at(i), SESDELETE);

      // output diff tag begin
      output_node(diff_old_end.get(), SESDELETE);

    }

    // output diff tag begin
    output_node(diff_old_end.get(), SESDELETE);

    rbuf_old->last_output = end_old;

  }

  if(end_new > begin_new) {

    for(unsigned int i = begin_new; i < end_new; ++i) {

      if(rbuf_new->nodes.at(i)->move) {

        srcdiff_move move(*this, i, SESINSERT);
        move.output();

        continue;

      }

      // output diff tag
      output_node(diff_new_start.get(), SESINSERT);

      output_node(rbuf_new->nodes.at(i), SESINSERT);

    // output diff tag begin
    output_node(diff_new_end.get(), SESINSERT);


    }

    // output diff tag begin
    output_node(diff_new_end.get(), SESINSERT);

    rbuf_new->last_output = end_new;

  }

  diff_old_start->properties = 0;
  diff_new_start->properties = 0;

}
