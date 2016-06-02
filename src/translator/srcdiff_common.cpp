#include <srcdiff_common.hpp>

#include <srcdiff_whitespace.hpp>
#include <srcdiff_compare.hpp>

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

    if(srcdiff_compare::node_compare(rbuf_original->nodes.at(i), rbuf_modified->nodes.at(j)) == 0) {

      output_node(rbuf_original->nodes.at(i), SES_COMMON);
        
    } else if(rbuf_original->nodes.at(i)->is_white_space() && rbuf_modified->nodes.at(j)->is_white_space()) {
      
      int olength = i;
      int nlength = j;

      for(; olength < oend && rbuf_original->nodes.at(olength)->is_white_space(); ++olength)
        ;

      for(; nlength < nend && rbuf_modified->nodes.at(nlength)->is_white_space(); ++nlength)
        ;

      //markup_whitespace(rbuf_original, olength, rbuf_modified, nlength, wstate);

      int opivot = olength - 1;
      int npivot = nlength - 1;

      for(; opivot > i && npivot > j && srcdiff_compare::node_compare(rbuf_original->nodes.at(opivot), rbuf_modified->nodes.at(npivot)) == 0; --opivot, --npivot)
        ;

      if(opivot < i || npivot < j) {

        opivot = olength;
        npivot = nlength;

      } else if(srcdiff_compare::node_compare(rbuf_original->nodes.at(opivot), rbuf_modified->nodes.at(npivot)) != 0) {

        ++opivot;
        ++npivot;
      }

        if(i < opivot) {

        output_node(diff_original_start, SES_DELETE);
        output_node(diff_ws_start, SES_DELETE);

        for(int k = i; k < opivot; ++k)
          output_node(rbuf_original->nodes.at(k), SES_DELETE);

        output_node(diff_ws_end, SES_DELETE);
        output_node(diff_original_end, SES_DELETE);

        }

        if(j < npivot) {

        output_node(diff_modified_start, SES_INSERT);
        output_node(diff_ws_start, SES_INSERT);

        for(int k = j; k < npivot; ++k)
          output_node(rbuf_modified->nodes.at(k), SES_INSERT);

        output_node(diff_ws_end, SES_INSERT);
        output_node(diff_modified_end, SES_INSERT);

        }

        if(opivot < olength) {

          //output_node(diff_common_start, SES_COMMON);

        for(int k = opivot; k < olength; ++k)
          output_node(rbuf_original->nodes.at(k), SES_COMMON);

        // output diff tag
        //output_node(diff_common_end, SES_COMMON);

        }

        i = olength - 1;

        j = nlength - 1;

    } else if(rbuf_original->nodes.at(i)->is_white_space()) {

      output_node(diff_original_start, SES_DELETE);
        output_node(diff_ws_start, SES_DELETE);

      for(; i < oend && rbuf_original->nodes.at(i)->is_white_space(); ++i)
        output_node(rbuf_original->nodes.at(i), SES_DELETE);


      output_node(diff_ws_end, SES_DELETE);
      output_node(diff_original_end, SES_DELETE);

      --i;
      --j;

    } else if(rbuf_modified->nodes.at(j)->is_white_space()) {

      output_node(diff_modified_start, SES_INSERT);
      output_node(diff_ws_start, SES_INSERT);


      for(; j < nend && rbuf_modified->nodes.at(j)->is_white_space(); ++j)
        output_node(rbuf_modified->nodes.at(j), SES_INSERT);


      output_node(diff_ws_end, SES_INSERT);
      output_node(diff_modified_end, SES_INSERT);

      --i;
      --j;

    } else if(rbuf_original->nodes.at(i)->is_text() && rbuf_modified->nodes.at(j)->is_text()) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_original = "";
      for(; i < oend && rbuf_original->nodes.at(i)->is_text(); ++i)
        text_original += rbuf_original->nodes.at(i)->content ? *rbuf_original->nodes.at(i)->content : "";

      std::string text_modified = "";
      for(; j < nend && rbuf_modified->nodes.at(j)->is_text(); ++j)
        text_modified += rbuf_modified->nodes.at(j)->content ? *rbuf_modified->nodes.at(j)->content : "";

      --i;
      --j;

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, text_original.c_str());
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, text_modified.c_str());

      int opos = 0;
      int npos = 0;
      for(; opos < (signed)text_original.size() && npos < (signed)text_modified.size();) {

        if(text_original[opos] == text_modified[npos]) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_original[opos]);
          output_char((xmlChar)text_original[opos], SES_COMMON);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_original[opos]) || isspace(text_modified[npos])) {

            if(isspace(text_original[opos])) {

              output_node(diff_original_start, SES_DELETE);

              for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_original[opos]);
                output_char((xmlChar)text_original[opos], SES_DELETE);
              }

              // output diff tag
              output_node(diff_original_end, SES_DELETE);

            }

            if(isspace(text_modified[npos])) {

              output_node(diff_modified_start, SES_INSERT);

              for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_modified[npos]);
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

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_original[opos]);
          output_char((xmlChar)text_original[opos], SES_DELETE);
        }

        // output diff tag
        output_node(diff_original_end, SES_DELETE);

      }

      if(npos < (signed)text_modified.size()) {
        
        output_node(diff_modified_start, SES_INSERT);

        for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_modified[npos]);
          output_char((xmlChar)text_modified[npos], SES_INSERT);
        }

        // output diff tag
        output_node(diff_modified_end, SES_INSERT);

      }


    } else {

      if(rbuf_original->nodes.at(i)->is_text())
        fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original->nodes.at(i)->content->c_str());
      else
        fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original->nodes.at(i)->name.c_str());
      if(rbuf_modified->nodes.at(j)->is_text())
        fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_modified->nodes.at(j)->content->c_str());
      else
        fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_modified->nodes.at(j)->name.c_str());

      std::cerr << "Original:\n";
      for(int pos = begin_original; pos < oend; ++pos)
        if(rbuf_original->nodes.at(pos)->is_text())
          std::cerr << *rbuf_original->nodes.at(pos)->content;
        else
          std::cerr << rbuf_original->nodes.at(pos)->name;
      std::cerr << '\n';

      std::cerr << "Modified:\n";
      for(int pos = begin_modified; pos < nend; ++pos)
        if(rbuf_modified->nodes.at(pos)->is_text())
          std::cerr << *rbuf_modified->nodes.at(pos)->content;
        else
          std::cerr << rbuf_modified->nodes.at(pos)->name;
      std::cerr << '\n';

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

    for( ; i < oend; ++i)
      output_node(rbuf_original->nodes.at(i), SES_DELETE);

    // output diff tag
    output_node(diff_original_end, SES_DELETE);

  } else if(j < nend) {

    output_node(diff_modified_start, SES_INSERT);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j)
      output_node(rbuf_modified->nodes.at(j), SES_INSERT);

    // output diff tag
    output_node(diff_modified_end, SES_INSERT);

  }

  rbuf_original->last_output = oend > (signed)rbuf_original->last_output ? oend : rbuf_original->last_output;
  rbuf_modified->last_output = nend > (signed)rbuf_modified->last_output ? nend : rbuf_modified->last_output;

  diff_original_start->properties.clear();
  diff_modified_start->properties.clear();

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
  output_node(diff_common_start, SES_COMMON);

  // output common nodes
  markup_common();

  // may need to take out all occurences afterwards except on pure deletes of this after something
  //output_statement(rbuf_original, rbuf_modified, wstate);

  // output common tag if needed
  output_node(diff_common_end, SES_COMMON);

}
