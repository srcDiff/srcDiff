#include <srcDiffCommon.hpp>
#include <srcDiffConstants.hpp>
#include <srcDiffUtility.hpp>
#include <srcDiffOutput.hpp>
#include <srcDiffWhiteSpace.hpp>

#include <string>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

extern xAttr diff_type;

srcdiff_common::srcdiff_common(const srcdiff_output & out, unsigned int end_old, unsigned int end_new)
: srcdiff_output(out), end_old(end_old), end_new(end_new) {}

/*

  Output of same syntactical entities, but possible whitespace differences.

*/
void srcdiff_common::markup_common() {

  int begin_old = rbuf_old.last_output;
  int begin_new = rbuf_new.last_output;

  int oend = end_old;
  int nend = end_new;

  // set attribute to change
  diff_type.value = whitespace;
  diff_old_start.properties = &diff_type;
  diff_new_start.properties = &diff_type;

  int i, j;
  for(i = begin_old, j = begin_new; i < oend && j < nend; ++i, ++j) {

    if(node_compare(rbuf_old.nodes.at(i), rbuf_new.nodes.at(j)) == 0) {

      output_node(rbuf_old.nodes.at(i), SESCOMMON);
        
    } else if(is_white_space(rbuf_old.nodes.at(i)) && is_white_space(rbuf_new.nodes.at(j))) {
      
      int olength = i;
      int nlength = j;

      for(; olength < oend && is_white_space(rbuf_old.nodes.at(olength)); ++olength)
        ;

      for(; nlength < nend && is_white_space(rbuf_new.nodes.at(nlength)); ++nlength)
        ;

      //markup_whitespace(rbuf_old, olength, rbuf_new, nlength, wstate);

      int opivot = olength - 1;
      int npivot = nlength - 1;

      for(; opivot > i && npivot > j && node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) == 0; --opivot, --npivot)
        ;

      if(opivot < i || npivot < j) {

        opivot = olength;
        npivot = nlength;

      } else if(node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) != 0) {

        ++opivot;
        ++npivot;
      }

        if(i < opivot) {

        output_node(&diff_old_start, SESDELETE);

        for(int k = i; k < opivot; ++k)
          output_node(rbuf_old.nodes.at(k), SESDELETE);

        // output diff tag
        output_node(&diff_old_end, SESDELETE);

        }

        if(j < npivot) {

        output_node(&diff_new_start, SESINSERT);

        for(int k = j; k < npivot; ++k)
          output_node(rbuf_new.nodes.at(k), SESINSERT);

        // output diff tag
        output_node(&diff_new_end, SESINSERT);

        }

        if(opivot < olength) {

          //output_node(&diff_common_start, SESCOMMON);

        for(int k = opivot; k < olength; ++k)
          output_node(rbuf_old.nodes.at(k), SESCOMMON);

        // output diff tag
        //output_node(&diff_common_end, SESCOMMON);

        }

        i = olength - 1;

        j = nlength - 1;

    } else if(is_white_space(rbuf_old.nodes.at(i))) {

      output_node(&diff_old_start, SESDELETE);
      // whitespace delete
      // output diff tag

      for(; i < oend && is_white_space(rbuf_old.nodes.at(i)); ++i)
        output_node(rbuf_old.nodes.at(i), SESDELETE);

      // output diff tag
      output_node(&diff_old_end, SESDELETE);

      --i;
      --j;

    } else if(is_white_space(rbuf_new.nodes.at(j))) {

      output_node(&diff_new_start, SESINSERT);
      //whitespace insert
      // output diff tag

      for(; j < nend && is_white_space(rbuf_new.nodes.at(j)); ++j)
        output_node(rbuf_new.nodes.at(j), SESINSERT);

      // output diff tag
      output_node(&diff_new_end, SESINSERT);

      --i;
      --j;

    } else if(is_text(rbuf_old.nodes.at(i)) && is_text(rbuf_new.nodes.at(j))) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_old = "";
      for(; i < oend && is_text(rbuf_old.nodes.at(i)); ++i)
        text_old += (const char *)rbuf_old.nodes.at(i)->content;

      std::string text_new = "";
      for(; j < nend && is_text(rbuf_new.nodes.at(j)); ++j)
        text_new += (const char *)rbuf_new.nodes.at(j)->content;

      --i;
      --j;

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, text_old.c_str());
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, text_new.c_str());

      int opos = 0;
      int npos = 0;
      for(; opos < (signed)text_old.size() && npos < (signed)text_new.size();) {

        if(text_old[opos] == text_new[npos]) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
          output_char((xmlChar)text_old[opos], SESCOMMON);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_old[opos]) || isspace(text_new[npos])) {

            if(isspace(text_old[opos])) {

              output_node(&diff_old_start, SESDELETE);

              for(; opos < (signed)text_old.size() && isspace(text_old[opos]); ++opos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
                output_char((xmlChar)text_old[opos], SESDELETE);
              }

              // output diff tag
              output_node(&diff_old_end, SESDELETE);

            }

            if(isspace(text_new[npos])) {

              output_node(&diff_new_start, SESINSERT);

              for(; npos < (signed)text_new.size() && isspace(text_new[npos]); ++npos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
                output_char((xmlChar)text_new[npos], SESINSERT);
              }

              // output diff tag
              output_node(&diff_new_end, SESINSERT);

            }

          }

        }

      }

      if(opos < (signed)text_old.size()) {

        output_node(&diff_old_start, SESDELETE);

        for(; opos < (signed)text_old.size() && isspace(text_old[opos]); ++opos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
          output_char((xmlChar)text_old[opos], SESDELETE);
        }

        // output diff tag
        output_node(&diff_old_end, SESDELETE);

      }

      if(npos < (signed)text_new.size()) {
        
        output_node(&diff_new_start, SESINSERT);

        for(; npos < (signed)text_new.size() && isspace(text_new[npos]); ++npos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
          output_char((xmlChar)text_new[npos], SESINSERT);
        }

        // output diff tag
        output_node(&diff_new_end, SESINSERT);

      }


    } else {

      fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)rbuf_old.nodes.at(i)->name);
      fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)rbuf_new.nodes.at(j)->name);

      // should never reach this state  This usually occurs when the two lines are not actually the same i.e. more than just whitespace
      fprintf(stderr, "ERROR\n");
      exit(1);
    }

  }

  // output leftover nodes
  if(i < oend) {

    output_node(&diff_old_start, SESDELETE);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i)
      output_node(rbuf_old.nodes.at(i), SESDELETE);

    // output diff tag
    output_node(&diff_old_end, SESDELETE);

  } else if(j < nend) {

    output_node(&diff_new_start, SESINSERT);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j)
      output_node(rbuf_new.nodes.at(j), SESINSERT);

    // output diff tag
    output_node(&diff_new_end, SESINSERT);

  }

  rbuf_old.last_output = oend > (signed)rbuf_old.last_output ? oend : rbuf_old.last_output;
  rbuf_new.last_output = nend > (signed)rbuf_new.last_output ? nend : rbuf_new.last_output;

  diff_old_start.properties = 0;
  diff_new_start.properties = 0;

}

/*

  Output common elements.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after has no newline,
  it is included and the following nodes are included if they have a new line.

*/

void srcdiff_common::output() {

  unsigned int oend = end_old;
  unsigned int nend = end_new;

  if(rbuf_old.last_output >= oend && rbuf_new.last_output >= nend)
    return;

  srcdiff_whitespace whitespace(*this);
  whitespace.output_white_space_all();

  if(rbuf_old.last_output >= oend && rbuf_new.last_output >= nend)
    return;

  // output common tag if needed
  output_node(&diff_common_start, SESCOMMON);

  // output common nodes
  markup_common();

  // may need to take out all occurences afterwards except on pure deletes of this after something
  //output_white_space_statement(rbuf_old, rbuf_new, wstate);

  // output common tag if needed
  output_node(&diff_common_end, SESCOMMON);

}
