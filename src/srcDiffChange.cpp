#include "srcDiffChange.hpp"
#include "srcDiffUtility.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "shortest_edit_script.h"

#include <string>

// global structures defined in main
extern std::vector<xmlNode *> nodes_old;
extern std::vector<xmlNode *> nodes_new;

// more external variables
extern xmlNode diff_common_start;
extern xmlNode diff_common_end;
extern xmlNode diff_old_start;
extern xmlNode diff_old_end;
extern xmlNode diff_new_start;
extern xmlNode diff_new_end;

/*

  Adds whitespace to a change. Then outputs the change.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after is not a newline,
  it is included and the following nodes are included if they have a new line.

*/
void output_change_white_space(reader_state & rbuf_old, unsigned int end_old
                               , reader_state & rbuf_new, unsigned int end_new
                               , writer_state & wstate) {

  int oend = end_old;
  int nend = end_new;

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

  advance_white_space_suffix(rbuf_old, oend, rbuf_new, nend);
  output_change(rbuf_old, oend, rbuf_new, nend, wstate);

  // need to make function that sets end and pass modified end to change then call to output rest
  output_white_space_all(rbuf_old, rbuf_new, wstate);

}

/*

  Outputs a syntactical diff. Beginning whitespace is narrowed and all
  whitespace even if it could be matches is treated as different.

*/
void output_change(reader_state & rbuf_old, unsigned int end_old
                   , reader_state & rbuf_new, unsigned int end_new
                   , writer_state & wstate) {

  unsigned int begin_old = rbuf_old.last_output;
  unsigned int begin_new = rbuf_new.last_output;

  if(end_old > begin_old && end_new > begin_new) {

    if(is_white_space(nodes_old.at(begin_old)) && is_white_space(nodes_new.at(begin_new))) {

      xmlChar * content_old = nodes_old.at(begin_old)->content;
      xmlChar * content_new = nodes_new.at(begin_new)->content;

      int size_old = strlen((const char *)content_old);
      int size_new = strlen((const char *)content_new);

      int offset_old = 0;
      int offset_new = 0;

      for(; offset_old < size_old && offset_new < size_new && content_old[offset_old] == content_new[offset_new]; ++offset_old, ++offset_new)
        ;

      output_text_as_node(rbuf_old, rbuf_new, (xmlChar *)strndup((const char *)content_old, offset_old), COMMON, wstate);

      if(offset_old < size_old) {

        // shrink
        nodes_old.at(begin_old)->content = content_old + offset_old;
        //node_sets_old->at(begin_old)->at(0)->content = (xmlChar *)strndup((const char *)(content_old + offset_old), size_old - offset_old);

      } else {

        nodes_old.at(begin_old)->content = (xmlChar *)"";
      }

      if(offset_new < size_new) {

        nodes_new.at(begin_new)->content = content_new + offset_new;

      } else {

        nodes_new.at(begin_new)->content = (xmlChar *)"";
      }


    }

  }

  if(end_old > begin_old) {

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

    for(unsigned int i = begin_old; i < end_old; ++i)
      output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    rbuf_old.last_output = end_old;

  }

  if(end_new > begin_new) {

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

    for(unsigned int i = begin_new; i < end_new; ++i)
      output_node(rbuf_old, rbuf_new, nodes_new.at(i), INSERT, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

    rbuf_new.last_output = end_new;

  }

}

void output_pure_operation_white_space(reader_state & rbuf_old, unsigned int end_old
                                       , reader_state & rbuf_new, unsigned int end_new
                                       , int operation, writer_state & wstate) {

  unsigned int oend = end_old;
  unsigned int nend = end_new;

  output_white_space_prefix(rbuf_old, rbuf_new, wstate);

  output_change(rbuf_old, oend, rbuf_new, nend, wstate);

  output_white_space_suffix(rbuf_old, rbuf_new, wstate);

  //  output_white_space_pure_statement_end(rbuf_old, rbuf_new, operation, wstate);

  //output_white_space_all(rbuf_old, rbuf_new, wstate);

}

