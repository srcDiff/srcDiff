#include <srcdiff_common.hpp>

#include <srcdiff_whitespace.hpp>
#include <srcdiff_compare.hpp>
#include <srcdiff_constants.hpp>

#include <string>

srcdiff_common::srcdiff_common(const srcdiff_output & out, unsigned int end_old, unsigned int end_new)
: srcdiff_output(out), end_old(end_old), end_new(end_new) {}

/*

  Output of same syntactical entities, but possible whitespace differences.

*/
void srcdiff_common::markup_common() {

  int begin_old = rbuf_old->last_output;
  int begin_new = rbuf_new->last_output;

  int oend = end_old;
  int nend = end_new;

  // set attribute to change
  diff_type->value = whitespace;
  diff_old_start->properties = diff_type.get();
  diff_new_start->properties = diff_type.get();

  int i, j;
  for(i = begin_old, j = begin_new; i < oend && j < nend; ++i, ++j) {

    if(srcdiff_compare::node_compare(rbuf_old->nodes.at(i), rbuf_new->nodes.at(j)) == 0) {

      output_node(rbuf_old->nodes.at(i), SESCOMMON);
        
    } else if(rbuf_old->nodes.at(i)->is_white_space() && rbuf_new->nodes.at(j)->is_white_space()) {
      
      int olength = i;
      int nlength = j;

      for(; olength < oend && rbuf_old->nodes.at(olength)->is_white_space(); ++olength)
        ;

      for(; nlength < nend && rbuf_new->nodes.at(nlength)->is_white_space(); ++nlength)
        ;

      //markup_whitespace(rbuf_old, olength, rbuf_new, nlength, wstate);

      int opivot = olength - 1;
      int npivot = nlength - 1;

      for(; opivot > i && npivot > j && srcdiff_compare::node_compare(rbuf_old->nodes.at(opivot), rbuf_new->nodes.at(npivot)) == 0; --opivot, --npivot)
        ;

      if(opivot < i || npivot < j) {

        opivot = olength;
        npivot = nlength;

      } else if(srcdiff_compare::node_compare(rbuf_old->nodes.at(opivot), rbuf_new->nodes.at(npivot)) != 0) {

        ++opivot;
        ++npivot;
      }

        if(i < opivot) {

        output_node(diff_old_start.get(), SESDELETE);

        for(int k = i; k < opivot; ++k)
          output_node(rbuf_old->nodes.at(k), SESDELETE);

        // output diff tag
        output_node(diff_old_end.get(), SESDELETE);

        }

        if(j < npivot) {

        output_node(diff_new_start.get(), SESINSERT);

        for(int k = j; k < npivot; ++k)
          output_node(rbuf_new->nodes.at(k), SESINSERT);

        // output diff tag
        output_node(diff_new_end.get(), SESINSERT);

        }

        if(opivot < olength) {

          //output_node(diff_common_start.get(), SESCOMMON);

        for(int k = opivot; k < olength; ++k)
          output_node(rbuf_old->nodes.at(k), SESCOMMON);

        // output diff tag
        //output_node(diff_common_end.get(), SESCOMMON);

        }

        i = olength - 1;

        j = nlength - 1;

    } else if(rbuf_old->nodes.at(i)->is_white_space()) {

      output_node(diff_old_start.get(), SESDELETE);
      // whitespace delete
      // output diff tag

      for(; i < oend && rbuf_old->nodes.at(i)->is_white_space(); ++i)
        output_node(rbuf_old->nodes.at(i), SESDELETE);

      // output diff tag
      output_node(diff_old_end.get(), SESDELETE);

      --i;
      --j;

    } else if(rbuf_new->nodes.at(j)->is_white_space()) {

      output_node(diff_new_start.get(), SESINSERT);
      //whitespace insert
      // output diff tag

      for(; j < nend && rbuf_new->nodes.at(j)->is_white_space(); ++j)
        output_node(rbuf_new->nodes.at(j), SESINSERT);

      // output diff tag
      output_node(diff_new_end.get(), SESINSERT);

      --i;
      --j;

    } else if(rbuf_old->nodes.at(i)->is_text() && rbuf_new->nodes.at(j)->is_text()) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_old = "";
      for(; i < oend && rbuf_old->nodes.at(i)->is_text(); ++i)
        text_old += rbuf_old->nodes.at(i)->content ? *rbuf_old->nodes.at(i)->content : "";

      std::string text_new = "";
      for(; j < nend && rbuf_new->nodes.at(j)->is_text(); ++j)
        text_new += rbuf_new->nodes.at(j)->content ? *rbuf_new->nodes.at(j)->content : "";

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

              output_node(diff_old_start.get(), SESDELETE);

              for(; opos < (signed)text_old.size() && isspace(text_old[opos]); ++opos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
                output_char((xmlChar)text_old[opos], SESDELETE);
              }

              // output diff tag
              output_node(diff_old_end.get(), SESDELETE);

            }

            if(isspace(text_new[npos])) {

              output_node(diff_new_start.get(), SESINSERT);

              for(; npos < (signed)text_new.size() && isspace(text_new[npos]); ++npos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
                output_char((xmlChar)text_new[npos], SESINSERT);
              }

              // output diff tag
              output_node(diff_new_end.get(), SESINSERT);

            }

          }

        }

      }

      if(opos < (signed)text_old.size()) {

        output_node(diff_old_start.get(), SESDELETE);

        for(; opos < (signed)text_old.size() && isspace(text_old[opos]); ++opos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
          output_char((xmlChar)text_old[opos], SESDELETE);
        }

        // output diff tag
        output_node(diff_old_end.get(), SESDELETE);

      }

      if(npos < (signed)text_new.size()) {
        
        output_node(diff_new_start.get(), SESINSERT);

        for(; npos < (signed)text_new.size() && isspace(text_new[npos]); ++npos) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
          output_char((xmlChar)text_new[npos], SESINSERT);
        }

        // output diff tag
        output_node(diff_new_end.get(), SESINSERT);

      }


    } else {

      fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old->nodes.at(i)->name.c_str());
      fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, rbuf_new->nodes.at(j)->name.c_str());

      // should never reach this state  This usually occurs when the two lines are not actually the same i.e. more than just whitespace
      fprintf(stderr, "ERROR\n");
      exit(1);
    }

  }

  // output leftover nodes
  if(i < oend) {

    output_node(diff_old_start.get(), SESDELETE);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i)
      output_node(rbuf_old->nodes.at(i), SESDELETE);

    // output diff tag
    output_node(diff_old_end.get(), SESDELETE);

  } else if(j < nend) {

    output_node(diff_new_start.get(), SESINSERT);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j)
      output_node(rbuf_new->nodes.at(j), SESINSERT);

    // output diff tag
    output_node(diff_new_end.get(), SESINSERT);

  }

  rbuf_old->last_output = oend > (signed)rbuf_old->last_output ? oend : rbuf_old->last_output;
  rbuf_new->last_output = nend > (signed)rbuf_new->last_output ? nend : rbuf_new->last_output;

  diff_old_start->properties = 0;
  diff_new_start->properties = 0;

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

  if(rbuf_old->last_output >= oend && rbuf_new->last_output >= nend)
    return;

  srcdiff_whitespace whitespace(*this);
  whitespace.output_all();

  if(rbuf_old->last_output >= oend && rbuf_new->last_output >= nend)
    return;

  // output common tag if needed
  output_node(diff_common_start.get(), SESCOMMON);

  // output common nodes
  markup_common();

  // may need to take out all occurences afterwards except on pure deletes of this after something
  //output_statement(rbuf_old, rbuf_new, wstate);

  // output common tag if needed
  output_node(diff_common_end.get(), SESCOMMON);

}
