#include <srcdiff_change.hpp>
#include <srcDiffConstants.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffWhiteSpace.hpp>
#include <shortest_edit_script.h>
#include <xmlrw.hpp>
#include <srcDiffMove.hpp>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

#include <string.h>
#include <string>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

extern xAttr diff_type;

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

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

}

/*

  Outputs a syntactical diff. Beginning whitespace is narrowed and all
  whitespace even if it could be matches is treated as different.

*/
void srcdiff_change::output() {

  unsigned int begin_old = rbuf_old.last_output;
  unsigned int begin_new = rbuf_new.last_output;

  if(end_old > begin_old && end_new > begin_new) {

    // set attribute to change
    diff_type.value = change;
    diff_old_start.properties = &diff_type;
    diff_new_start.properties = &diff_type;

    if(0 && is_white_space(rbuf_old.nodes.at(begin_old)) && is_white_space(rbuf_new.nodes.at(begin_new))) {

      const char * content_old = rbuf_old.nodes.at(begin_old)->content;
      const char * content_new = rbuf_new.nodes.at(begin_new)->content;

      int size_old = strlen((const char *)content_old);
      int size_new = strlen((const char *)content_new);

      int offset_old = 0;
      int offset_new = 0;

      for(; offset_old < size_old && offset_new < size_new && content_old[offset_old] == content_new[offset_new]; ++offset_old, ++offset_new)
        ;

      char * content = strndup((const char *)content_old, offset_old);
      
      output_text_as_node(content, SESCOMMON);

      free(content);

      if(offset_old < size_old) {

        // shrink
        rbuf_old.nodes.at(begin_old)->content = content_old + offset_old;
        //node_sets_old->at(begin_old)->at(0)->content = (xmlChar *)strndup((const char *)(content_old + offset_old), size_old - offset_old);

      } else {

        rbuf_old.nodes.at(begin_old)->content = "";
      }

      if(offset_new < size_new) {

        rbuf_new.nodes.at(begin_new)->content = content_new + offset_new;

      } else {

        rbuf_new.nodes.at(begin_new)->content = "";
      }


    }

  }

  if(end_old > begin_old) {


    for(unsigned int i = begin_old; i < end_old; ++i) {

      if(rbuf_old.nodes.at(i)->move) {

        srcdiff_move move(*this, i, SESDELETE);
        move.output();

        continue;

      }

      // output diff tag begin
      output_node(&diff_old_start, SESDELETE);

      output_node(rbuf_old.nodes.at(i), SESDELETE);

      // output diff tag begin
      output_node(&diff_old_end, SESDELETE);

    }

    // output diff tag begin
    output_node(&diff_old_end, SESDELETE);

    rbuf_old.last_output = end_old;

  }

  if(end_new > begin_new) {

    for(unsigned int i = begin_new; i < end_new; ++i) {

      if(rbuf_new.nodes.at(i)->move) {

        srcdiff_move move(*this, i, SESINSERT);
        move.output();

        continue;

      }

      // output diff tag
      output_node(&diff_new_start, SESINSERT);

      output_node(rbuf_new.nodes.at(i), SESINSERT);

    // output diff tag begin
    output_node(&diff_new_end, SESINSERT);


    }

    // output diff tag begin
    output_node(&diff_new_end, SESINSERT);

    rbuf_new.last_output = end_new;

  }

  diff_old_start.properties = 0;
  diff_new_start.properties = 0;

}
