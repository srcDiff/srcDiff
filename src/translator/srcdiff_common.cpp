// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_common.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_common.hpp>

#include <srcdiff_whitespace.hpp>

#include <string>

srcdiff_common::srcdiff_common(const srcdiff_output & out, unsigned int end_original, unsigned int end_modified)
: srcdiff_output(out), end_original(end_original), end_modified(end_modified) {}

/*

  Output of same syntactical entities, but possible whitespace differences.

*/
void srcdiff_common::markup_common() {

  int begin_original = rbuf_original->last_output;
  int begin_modified = rbuf_modified->last_output;

  int oend = end_original;
  int nend = end_modified;

  // set attribute to change
  int i, j;
  for(i = begin_original, j = begin_modified; i < oend && j < nend; ++i, ++j) {

    if(*rbuf_original->nodes.at(i) == *rbuf_modified->nodes.at(j)) {

      output_node(rbuf_original->nodes.at(i), rbuf_modified->nodes.at(j), SES_COMMON);
        
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

        output_node(diff_original_start, SES_DELETE);
        output_node(diff_ws_start, SES_DELETE);

        for(int k = i; k < opivot; ++k) {
          output_node(rbuf_original->nodes.at(k), SES_DELETE);
        }

        output_node(diff_ws_end, SES_DELETE);
        output_node(diff_original_end, SES_DELETE);

        }

        if(j < npivot) {

        output_node(diff_modified_start, SES_INSERT);
        output_node(diff_ws_start, SES_INSERT);

        for(int k = j; k < npivot; ++k) {
          output_node(rbuf_modified->nodes.at(k), SES_INSERT);
        }

        output_node(diff_ws_end, SES_INSERT);
        output_node(diff_modified_end, SES_INSERT);

        }

        if(opivot < olength) {

        for(int k = opivot; k < olength; ++k) {
          output_node(rbuf_original->nodes.at(k), SES_COMMON);
        }

        }

        i = olength - 1;

        j = nlength - 1;

    } else if(rbuf_original->nodes.at(i)->is_whitespace()) {

      output_node(diff_original_start, SES_DELETE);
        output_node(diff_ws_start, SES_DELETE);

      for(; i < oend && rbuf_original->nodes.at(i)->is_whitespace(); ++i) {
        output_node(rbuf_original->nodes.at(i), SES_DELETE);
      }


      output_node(diff_ws_end, SES_DELETE);
      output_node(diff_original_end, SES_DELETE);

      --i;
      --j;

    } else if(rbuf_modified->nodes.at(j)->is_whitespace()) {

      output_node(diff_modified_start, SES_INSERT);
      output_node(diff_ws_start, SES_INSERT);


      for(; j < nend && rbuf_modified->nodes.at(j)->is_whitespace(); ++j) {
        output_node(rbuf_modified->nodes.at(j), SES_INSERT);
      }


      output_node(diff_ws_end, SES_INSERT);
      output_node(diff_modified_end, SES_INSERT);

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

          output_char((xmlChar)text_original[opos], SES_COMMON);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_original[opos]) || isspace(text_modified[npos])) {

            if(isspace(text_original[opos])) {

              output_node(diff_original_start, SES_DELETE);

              for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

                output_char((xmlChar)text_original[opos], SES_DELETE);
              }

              // output diff tag
              output_node(diff_original_end, SES_DELETE);

            }

            if(isspace(text_modified[npos])) {

              output_node(diff_modified_start, SES_INSERT);

              for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

                output_char((xmlChar)text_modified[npos], SES_INSERT);
              }

              // output diff tag
              output_node(diff_modified_end, SES_INSERT);

            }

          }

        }

      }

      if(opos < (signed)text_original.size()) {

        output_node(diff_original_start, SES_DELETE);

        for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

          output_char((xmlChar)text_original[opos], SES_DELETE);
        }

        // output diff tag
        output_node(diff_original_end, SES_DELETE);

      }

      if(npos < (signed)text_modified.size()) {
        
        output_node(diff_modified_start, SES_INSERT);

        for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

          output_char((xmlChar)text_modified[npos], SES_INSERT);
        }

        // output diff tag
        output_node(diff_modified_end, SES_INSERT);

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

    output_node(diff_original_start, SES_DELETE);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i) {
      output_node(rbuf_original->nodes.at(i), SES_DELETE);
    }

    // output diff tag
    output_node(diff_original_end, SES_DELETE);

  } else if(j < nend) {

    output_node(diff_modified_start, SES_INSERT);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j) {
      output_node(rbuf_modified->nodes.at(j), SES_INSERT);
    }

    // output diff tag
    output_node(diff_modified_end, SES_INSERT);

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

void srcdiff_common::output() {

  unsigned int oend = end_original;
  unsigned int nend = end_modified;

  if(rbuf_original->last_output >= oend && rbuf_modified->last_output >= nend)
    return;

  srcdiff_whitespace whitespace(*this);
  whitespace.output_all();

  if(rbuf_original->last_output >= oend && rbuf_modified->last_output >= nend)
    return;

  // output common tag if needed
  if(rbuf_original->last_output >= oend || rbuf_modified->last_output >= nend
     || rbuf_original->nodes.at(rbuf_original->last_output)->is_temporary() == rbuf_modified->nodes.at(rbuf_modified->last_output)->is_temporary()) {
    output_node(diff_common_start, SES_COMMON);
  }

  // output common nodes
  markup_common();

  // may need to take out all occurences afterwards except on pure deletes of this after something
  //output_statement(rbuf_original, rbuf_modified, wstate);

  // output common tag if needed
  if(rbuf_original->last_output >= oend || rbuf_modified->last_output >= nend
     || rbuf_original->nodes.at(rbuf_original->last_output)->is_temporary() == rbuf_modified->nodes.at(rbuf_modified->last_output)->is_temporary()) {
    output_node(diff_common_end, SES_COMMON);
  }

}
