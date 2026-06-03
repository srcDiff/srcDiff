// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file common_stream.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <common_stream.hpp>

#include <whitespace_stream.hpp>

#include <string>

namespace srcdiff {

common_stream::common_stream(const output_stream& out, unsigned int end_original, unsigned int end_modified)
: output_stream(out), end_original(end_original), end_modified(end_modified) {}

/*

  Output of same syntactical entities, but possible whitespace differences.

*/
void common_stream::markup_common() {

  int begin_original = rbuf_original->last_output;
  int begin_modified = rbuf_modified->last_output;

  int oend = end_original;
  int nend = end_modified;

  // set attribute to change
  int i, j;
  for(i = begin_original, j = begin_modified; i < oend && j < nend; ++i, ++j) {

    if(*rbuf_original->nodes.at(i) == *rbuf_modified->nodes.at(j)) {

      if(rbuf_original->nodes.at(i)->get_attributes() != rbuf_modified->nodes.at(j)->get_attributes()) {
        rbuf_original->nodes.at(i)->merge_attributes(rbuf_modified->nodes.at(j)->get_attributes());
      }
      output_node(rbuf_original->nodes.at(i), rbuf_modified->nodes.at(j), COMMON);
        
    } else if(rbuf_original->nodes.at(i)->is_whitespace() && rbuf_modified->nodes.at(j)->is_whitespace()) {
      
      int olength = i;
      int nlength = j;

      for(; olength < oend && rbuf_original->nodes.at(olength)->is_whitespace(); ++olength)
        ;

      for(; nlength < nend && rbuf_modified->nodes.at(nlength)->is_whitespace(); ++nlength)
        ;


      int opivot = olength - 1;
      int npivot = nlength - 1;

      for(; opivot > i && npivot > j && *rbuf_original->nodes.at(opivot) == *rbuf_modified->nodes.at(npivot); --opivot, --npivot)
        ;

      if(opivot < i || npivot < j) {

        opivot = olength;
        npivot = nlength;

      } else if(*rbuf_original->nodes.at(opivot) != *rbuf_modified->nodes.at(npivot)) {

        ++opivot;
        ++npivot;
      }

        if(i < opivot) {

        output_node(diff_original_start, DELETE);
        output_node(diff_ws_start, DELETE);

        for(int k = i; k < opivot; ++k) {
          output_node(rbuf_original->nodes.at(k), DELETE);
        }

        output_node(diff_ws_end, DELETE);
        output_node(diff_original_end, DELETE);

        }

        if(j < npivot) {

        output_node(diff_modified_start, INSERT);
        output_node(diff_ws_start, INSERT);

        for(int k = j; k < npivot; ++k) {
          output_node(rbuf_modified->nodes.at(k), INSERT);
        }

        output_node(diff_ws_end, INSERT);
        output_node(diff_modified_end, INSERT);

        }

        if(opivot < olength) {

        for(int k = opivot; k < olength; ++k) {
          output_node(rbuf_original->nodes.at(k), COMMON);
        }

        }

        i = olength - 1;

        j = nlength - 1;

    } else if(rbuf_original->nodes.at(i)->is_whitespace()) {

      output_node(diff_original_start, DELETE);
        output_node(diff_ws_start, DELETE);

      for(; i < oend && rbuf_original->nodes.at(i)->is_whitespace(); ++i) {
        output_node(rbuf_original->nodes.at(i), DELETE);
      }


      output_node(diff_ws_end, DELETE);
      output_node(diff_original_end, DELETE);

      --i;
      --j;

    } else if(rbuf_modified->nodes.at(j)->is_whitespace()) {

      output_node(diff_modified_start, INSERT);
      output_node(diff_ws_start, INSERT);


      for(; j < nend && rbuf_modified->nodes.at(j)->is_whitespace(); ++j) {
        output_node(rbuf_modified->nodes.at(j), INSERT);
      }


      output_node(diff_ws_end, INSERT);
      output_node(diff_modified_end, INSERT);

      --i;
      --j;

    } else if(rbuf_original->nodes.at(i)->is_text() && rbuf_modified->nodes.at(j)->is_text()) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_original = "";
      for(; i < oend && rbuf_original->nodes.at(i)->is_text(); ++i) {
        text_original += rbuf_original->nodes.at(i)->get_content() ? *rbuf_original->nodes.at(i)->get_content() : "";
      }

      std::string text_modified = "";
      for(; j < nend && rbuf_modified->nodes.at(j)->is_text(); ++j) {
        text_modified += rbuf_modified->nodes.at(j)->get_content() ? *rbuf_modified->nodes.at(j)->get_content() : "";
      }

      --i;
      --j;

      int opos = 0;
      int npos = 0;
      for(; opos < (signed)text_original.size() && npos < (signed)text_modified.size();) {

        if(text_original[opos] == text_modified[npos]) {

          output_char((xmlChar)text_original[opos], COMMON);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_original[opos]) || isspace(text_modified[npos])) {

            if(isspace(text_original[opos])) {

              output_node(diff_original_start, DELETE);

              for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

                output_char((xmlChar)text_original[opos], DELETE);
              }

              // output diff tag
              output_node(diff_original_end, DELETE);

            }

            if(isspace(text_modified[npos])) {

              output_node(diff_modified_start, INSERT);

              for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

                output_char((xmlChar)text_modified[npos], INSERT);
              }

              // output diff tag
              output_node(diff_modified_end, INSERT);

            }

          }

        }

      }

      if(opos < (signed)text_original.size()) {

        output_node(diff_original_start, DELETE);

        for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

          output_char((xmlChar)text_original[opos], DELETE);
        }

        // output diff tag
        output_node(diff_original_end, DELETE);

      }

      if(npos < (signed)text_modified.size()) {
        
        output_node(diff_modified_start, INSERT);

        for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

          output_char((xmlChar)text_modified[npos], INSERT);
        }

        // output diff tag
        output_node(diff_modified_end, INSERT);

      }


    } else {

#if DEBUG
      std::cerr << "Original:\n";
      for(int pos = begin_original; pos < oend; ++pos) {
        std::cerr << *rbuf_original->nodes.at(pos) << '\n';
      }
      std::cerr << '\n';

      std::cerr << "Modified:\n";
      for(int pos = begin_modified; pos < nend; ++pos) {
        std::cerr << *rbuf_modified->nodes.at(pos) << '\n';
      }
      std::cerr << '\n';
#endif

      // should never reach this state  This usually occurs when the two lines are not actually the same i.e. more than just whitespace
      fprintf(stderr, "Fatal Error Occurred\n");
      exit(1);
    }

  }

  // output leftover nodes
  if(i < oend) {

    output_node(diff_original_start, DELETE);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i) {
      output_node(rbuf_original->nodes.at(i), DELETE);
    }

    // output diff tag
    output_node(diff_original_end, DELETE);

  } else if(j < nend) {

    output_node(diff_modified_start, INSERT);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j) {
      output_node(rbuf_modified->nodes.at(j), INSERT);
    }

    // output diff tag
    output_node(diff_modified_end, INSERT);

  }

  rbuf_original->last_output = oend > (signed)rbuf_original->last_output ? oend : rbuf_original->last_output;
  rbuf_modified->last_output = nend > (signed)rbuf_modified->last_output ? nend : rbuf_modified->last_output;

  diff_original_start->clear_attributes();
  diff_modified_start->clear_attributes();

}

/*

  Output common elements.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after has no newline,
  it is included and the following nodes are included if they have a new line.

*/

void common_stream::output() {

  unsigned int oend = end_original;
  unsigned int nend = end_modified;

  if(rbuf_original->last_output >= oend && rbuf_modified->last_output >= nend)
    return;

  whitespace_stream whitespace(*this);
  whitespace.output_all();

  if(rbuf_original->last_output >= oend && rbuf_modified->last_output >= nend)
    return;

  // output common tag if needed
  if(rbuf_original->last_output >= oend || rbuf_modified->last_output >= nend
     || rbuf_original->nodes.at(rbuf_original->last_output)->is_temporary() == rbuf_modified->nodes.at(rbuf_modified->last_output)->is_temporary()) {
    output_node(diff_common_start, COMMON);
  }

  // output common nodes
  markup_common();

  // may need to take out all occurences afterwards except on pure deletes of this after something
  //output_statement(rbuf_original, rbuf_modified, wstate);

  // output common tag if needed
  if(rbuf_original->last_output >= oend || rbuf_modified->last_output >= nend
     || rbuf_original->nodes.at(rbuf_original->last_output)->is_temporary() == rbuf_modified->nodes.at(rbuf_modified->last_output)->is_temporary()) {
    output_node(diff_common_end, COMMON);
  }

}

}
