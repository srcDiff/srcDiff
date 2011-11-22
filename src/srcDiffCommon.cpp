/*

  Output of same syntactical entities, but possible whitespace differences.

*/
void markup_whitespace(reader_state & rbuf_old, unsigned int end_old, reader_state & rbuf_new, unsigned int end_new, writer_state & wstate) {

  int begin_old = rbuf_old.last_output;
  int begin_new = rbuf_new.last_output;

  int oend = end_old;
  int nend = end_new;

  int i, j;
  for(i = begin_old, j = begin_new; i < oend && j < nend; ++i, ++j) {

    if(node_compare(nodes_old.at(i), nodes_new.at(j)) == 0)

      output_node(rbuf_old, rbuf_new, nodes_old.at(i), COMMON, wstate);

    else if(is_white_space(nodes_old.at(i)) && is_white_space(nodes_new.at(j))) {

      int olength = i;
      int nlength = j;

      for(; olength < oend && is_white_space(nodes_old.at(olength)); ++olength)
        ;

      for(; nlength < nend && is_white_space(nodes_new.at(nlength)); ++nlength)
        ;

      int opivot = olength - 1;
      int npivot = nlength - 1;

      for(; opivot > i && npivot > j && node_compare(nodes_old.at(opivot), nodes_new.at(npivot)) == 0; --opivot, --npivot)
        ;

      if(opivot < i || npivot < j) {

        opivot = olength;
        npivot = nlength;

      } else if(node_compare(nodes_old.at(opivot), nodes_new.at(npivot)) != 0) {

        ++opivot;
        ++npivot;
      }

        if(i < opivot) {

        output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

        for(int k = i; k < opivot; ++k)
        output_node(rbuf_old, rbuf_new, nodes_old.at(k), DELETE, wstate);

        // output diff tag
        output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

        }

        if(j < npivot) {

        output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

        for(int k = j; k < npivot; ++k)
        output_node(rbuf_old, rbuf_new, nodes_new.at(k), INSERT, wstate);

        // output diff tag
        output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

        }

        if(opivot < olength) {

          //output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);

        for(int k = opivot; k < olength; ++k)
        output_node(rbuf_old, rbuf_new, nodes_old.at(k), COMMON, wstate);

        // output diff tag
        //output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

        }

        i = olength - 1;
        j = nlength - 1;

    } else if(is_white_space(nodes_old.at(i))) {

      output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);
      // whitespace delete
      // output diff tag

      output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

      // output diff tag
      output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

      --j;

    } else if(is_white_space(nodes_new.at(j))) {

      output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);
      //whitespace insert
      // output diff tag

      output_node(rbuf_old, rbuf_new, nodes_new.at(j), INSERT, wstate);

      // output diff tag
      output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

      --i;

    } else if(is_text(nodes_old.at(i)) && is_text(nodes_new.at(j))) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_old = "";
      for(; i < oend && is_text(nodes_old.at(i)); ++i)
        text_old += (const char *)nodes_old.at(i)->content;

      std::string text_new = "";
      for(; j < nend && is_text(nodes_new.at(j)); ++j)
        text_new += (const char *)nodes_new.at(j)->content;

      --i;
      --j;

      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, text_old.c_str());
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, text_new.c_str());

      for(int opos = 0, npos = 0; opos < (signed)text_old.size() && npos < (signed)text_new.size();) {

        if(text_old[opos] == text_new[npos]) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
          output_text_as_node(rbuf_old, rbuf_new, (xmlChar *)&text_old[opos], COMMON, wstate);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_old[opos]) || isspace(text_new[npos])) {

            if(isspace(text_old[opos])) {

              output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

              for(; opos < (signed)text_old.size() && isspace(text_old[opos]); ++opos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
                output_text_as_node(rbuf_old, rbuf_new, (xmlChar *)&text_old[opos], DELETE, wstate);
              }

              // output diff tag
              output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

            }

            if(isspace(text_new[npos])) {

              output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

              for(; npos < (signed)text_new.size() && isspace(text_new[npos]); ++npos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
                output_text_as_node(rbuf_old, rbuf_new, (xmlChar *)&text_new[npos], INSERT, wstate);
              }

              // output diff tag
              output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

            }

          }

        }

      }

    } else {

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes_old.at(i)->name);
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes_new.at(i)->name);

      // should never reach this state  This usually occurs when the two lines are not actually the same i.e. more than just whitespace
      fprintf(stderr, "ERROR");
      exit(1);
    }

  }

  // output leftover nodes
  if(i < oend) {

    output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i)
      output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

  } else if(j < nend) {

    output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j)
      output_node(rbuf_old, rbuf_new, nodes_new.at(j), INSERT, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

  }

  rbuf_old.last_output = oend > (signed)rbuf_old.last_output ? oend : rbuf_old.last_output;
  rbuf_new.last_output = nend > (signed)rbuf_new.last_output ? nend : rbuf_new.last_output;

}

