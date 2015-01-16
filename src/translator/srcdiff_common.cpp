#include <srcdiff_common.hpp>

#include <srcdiff_whitespace.hpp>
#include <srcdiff_compare.hpp>

#include <string>

const boost::optional<std::string> srcdiff_common::whitespace("whitespace");

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
  diff_type->value = whitespace;
  diff_original_start->properties.push_back(*diff_type.get());
  diff_modified_start->properties.push_back(*diff_type.get());

  int i, j;
  for(i = begin_original, j = begin_modified; i < oend && j < nend; ++i, ++j) {

    if(srcdiff_compare::node_compare(rbuf_original->nodes.at(i), rbuf_modified->nodes.at(j)) == 0) {

      output_node(rbuf_original->nodes.at(i), SESCOMMON);
        
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

        output_node(diff_original_start, SESDELETE);

        for(int k = i; k < opivot; ++k)
          output_node(rbuf_original->nodes.at(k), SESDELETE);

        // output diff tag
        output_node(diff_original_end, SESDELETE);

        }

        if(j < npivot) {

        output_node(diff_modified_start, SESINSERT);

        for(int k = j; k < npivot; ++k)
          output_node(rbuf_modified->nodes.at(k), SESINSERT);

        // output diff tag
        output_node(diff_modified_end, SESINSERT);

        }

        if(opivot < olength) {

          //output_node(diff_common_start, SESCOMMON);

        for(int k = opivot; k < olength; ++k)
          output_node(rbuf_original->nodes.at(k), SESCOMMON);

        // output diff tag
        //output_node(diff_common_end, SESCOMMON);

        }

        i = olength - 1;

        j = nlength - 1;

    } else if(rbuf_original->nodes.at(i)->is_white_space()) {

      output_node(diff_original_start, SESDELETE);
      // whitespace delete
      // output diff tag

      for(; i < oend && rbuf_original->nodes.at(i)->is_white_space(); ++i)
        output_node(rbuf_original->nodes.at(i), SESDELETE);

      // output diff tag
      output_node(diff_original_end, SESDELETE);

      --i;
      --j;

    } else if(rbuf_modified->nodes.at(j)->is_white_space()) {

      output_node(diff_modified_start, SESINSERT);
      //whitespace insert
      // output diff tag

      for(; j < nend && rbuf_modified->nodes.at(j)->is_white_space(); ++j)
        output_node(rbuf_modified->nodes.at(j), SESINSERT);

      // output diff tag
      output_node(diff_modified_end, SESINSERT);

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
          output_char((xmlChar)text_original[opos], SESCOMMON);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_original[opos]) || isspace(text_modified[npos])) {

            if(isspace(text_original[opos])) {

              output_node(diff_original_start, SESDELETE);

              for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_original[opos]);
                output_char((xmlChar)text_original[opos], SESDELETE);
              }

              // output diff tag
              output_node(diff_original_end, SESDELETE);

            }

            if(isspace(text_modified[npos])) {

              output_node(diff_modified_start, SESINSERT);

              for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_modified[npos]);
                output_char((xmlChar)text_modified[npos], SESINSERT);
              }

              // output diff tag
              output_node(diff_modified_end, SESINSERT);

            }

          }

        }

      }

      if(opos < (signed)text_original.size()) {

        output_node(diff_original_start, SESDELETE);

        for(; opos < (signed)text_original.size() && isspace(text_original[opos]); ++opos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_original[opos]);
          output_char((xmlChar)text_original[opos], SESDELETE);
        }

        // output diff tag
        output_node(diff_original_end, SESDELETE);

      }

      if(npos < (signed)text_modified.size()) {
        
        output_node(diff_modified_start, SESINSERT);

        for(; npos < (signed)text_modified.size() && isspace(text_modified[npos]); ++npos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_modified[npos]);
          output_char((xmlChar)text_modified[npos], SESINSERT);
        }

        // output diff tag
        output_node(diff_modified_end, SESINSERT);

      }


    } else {

      fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_original->nodes.at(i)->name.c_str());
      fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_modified->nodes.at(j)->name.c_str());

      // should never reach this state  This usually occurs when the two lines are not actually the same i.e. more than just whitespace
      fprintf(stderr, "ERROR\n");
      exit(1);
    }

  }

  // output leftover nodes
  if(i < oend) {

    output_node(diff_original_start, SESDELETE);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i)
      output_node(rbuf_original->nodes.at(i), SESDELETE);

    // output diff tag
    output_node(diff_original_end, SESDELETE);

  } else if(j < nend) {

    output_node(diff_modified_start, SESINSERT);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j)
      output_node(rbuf_modified->nodes.at(j), SESINSERT);

    // output diff tag
    output_node(diff_modified_end, SESINSERT);

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
  output_node(diff_common_start, SESCOMMON);

  // output common nodes
  markup_common();

  // may need to take out all occurences afterwards except on pure deletes of this after something
  //output_statement(rbuf_original, rbuf_modified, wstate);

  // output common tag if needed
  output_node(diff_common_end, SESCOMMON);

}
