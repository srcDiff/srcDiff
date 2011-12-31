#include "srcDiffWhiteSpace.hpp"
#include "shortest_edit_script.h"
#include "srcDiffUtility.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffOutput.hpp"

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

extern xAttr diff_type;
extern const char * change;
extern const char * whitespace;

void output_white_space_nested(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , int operation, writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old.nodes.size() && nend < rbuf_new.nodes.size()
        && is_white_space(rbuf_old.nodes.at(oend)) && is_white_space(rbuf_new.nodes.at(nend))
        ; ++oend)
    ;

  if(operation == DELETE)
    for(; oend < rbuf_old.nodes.size() && is_white_space(rbuf_old.nodes.at(oend)); ++oend) {
    }
  else
    for(; nend < rbuf_new.nodes.size() && is_white_space(rbuf_new.nodes.at(nend)); ++nend) {
    }

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}


void output_white_space_statement(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old.nodes.size() && is_white_space(rbuf_old.nodes.at(oend)) && !is_new_line(rbuf_old.nodes.at(oend)); ++oend)
    ;

  for(; nend < rbuf_new.nodes.size() && is_white_space(rbuf_new.nodes.at(nend)) && !is_new_line(rbuf_new.nodes.at(nend)); ++nend)
    ;

  if(oend < rbuf_old.nodes.size() && is_new_line(rbuf_old.nodes.at(oend)))
    ++oend;

  if(nend < rbuf_new.nodes.size() && is_new_line(rbuf_new.nodes.at(nend)))
    ++nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_all(reader_state & rbuf_old
                            , reader_state & rbuf_new
                            , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old.nodes.size() && is_white_space(rbuf_old.nodes.at(oend)); ++oend)
    ;

  for(; nend < rbuf_new.nodes.size() && is_white_space(rbuf_new.nodes.at(nend)); ++nend)
    ;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_pure_statement_end(reader_state & rbuf_old
                                           , reader_state & rbuf_new
                                           , int operation
                                           , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  if(operation == DELETE) {
    if(oend < rbuf_old.nodes.size() && is_new_line(rbuf_old.nodes.at(oend)))
      ++oend;
  }
  else if(operation == INSERT)
    if(nend < rbuf_new.nodes.size() && is_new_line(rbuf_new.nodes.at(nend)))
      ++nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_match_most(reader_state & rbuf_old
                                   , reader_state & rbuf_new
                                   , int operation
                                   , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end

  for(; oend < rbuf_old.nodes.size() && nend < rbuf_new.nodes.size()
        && is_white_space(rbuf_old.nodes.at(oend)) && is_white_space(rbuf_new.nodes.at(nend))
        && node_compare(rbuf_old.nodes.at(oend), rbuf_new.nodes.at(nend)) == 0; ++oend, ++nend)
    ;

  if(rbuf_old.last_output < oend && (is_white_space(rbuf_old.nodes.at(oend - 1)) && !is_new_line(rbuf_old.nodes.at(oend - 1))))
    --oend;

  if(rbuf_new.last_output < nend && (is_white_space(rbuf_new.nodes.at(nend - 1)) && !is_new_line(rbuf_new.nodes.at(nend - 1))))
    --nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_most(reader_state & rbuf_old
                             , reader_state & rbuf_new
                             , writer_state & wstate) {

  unsigned int oend = rbuf_old.last_output;
  unsigned int nend = rbuf_new.last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old.nodes.size() && is_white_space(rbuf_old.nodes.at(oend)); ++oend)
    ;

  for(; nend < rbuf_new.nodes.size() && is_white_space(rbuf_new.nodes.at(nend)); ++nend)
    ;

  if(rbuf_old.last_output < oend && (is_white_space(rbuf_old.nodes.at(oend - 1)) && !is_new_line(rbuf_old.nodes.at(oend - 1))))
    --oend;

  if(rbuf_new.last_output < nend && (is_white_space(rbuf_new.nodes.at(nend - 1)) && !is_new_line(rbuf_new.nodes.at(nend - 1))))
    --nend;

  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

}

void output_white_space_prefix(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , writer_state & wstate) {

  unsigned int ostart = rbuf_old.last_output;
  unsigned int nstart = rbuf_new.last_output;
  unsigned int oend = ostart;
  unsigned int nend = nstart;

  // advance whitespace while matches
  for(; oend < rbuf_old.nodes.size() && nend < rbuf_new.nodes.size()
        && is_white_space(rbuf_old.nodes.at(oend)) && is_white_space(rbuf_new.nodes.at(nend))
        && node_compare(rbuf_old.nodes.at(oend), rbuf_new.nodes.at(nend)) == 0; ++oend, ++nend)
    ;
  /*

    Change back to if's when not collecting whitespace as single nodes

  */
  // may only match here, but belongs as part of pure change
  unsigned int save_oend = oend;
  unsigned int save_nend = nend;

  while(rbuf_old.last_output < oend && (is_white_space(rbuf_old.nodes.at(oend - 1)) && !is_new_line(rbuf_old.nodes.at(oend - 1))))
    --oend;

  while(rbuf_new.last_output < nend && (is_white_space(rbuf_new.nodes.at(nend - 1)) && !is_new_line(rbuf_new.nodes.at(nend - 1))))
    --nend;

  if(oend > 0 && !is_new_line(rbuf_old.nodes.at(oend - 1)))
    oend = save_oend;

  if(nend > 0 && !is_new_line(rbuf_new.nodes.at(nend - 1)))
    nend = save_nend;

  if(ostart < oend) {

  output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);

  for(unsigned int i = ostart; i < oend; ++i)
    output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(i), COMMON, wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

  }

  rbuf_old.last_output = oend > rbuf_old.last_output ? oend : rbuf_old.last_output;
  rbuf_new.last_output = nend > rbuf_new.last_output ? nend : rbuf_new.last_output;

}

void output_white_space_suffix(reader_state & rbuf_old
                               , reader_state & rbuf_new
                               , writer_state & wstate) {

  int ostart = rbuf_old.last_output;
  int nstart = rbuf_new.last_output;
  int oend = ostart;
  int nend = nstart;

  // advance all whitespace
  for(; oend < (signed)rbuf_old.nodes.size() && is_white_space(rbuf_old.nodes.at(oend)); ++oend)
    ;

  for(; nend < (signed)rbuf_new.nodes.size() && is_white_space(rbuf_new.nodes.at(nend)); ++nend)
    ;

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart
        && node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) == 0; --opivot, --npivot)
    ;

  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) != 0) {
    ++opivot;
    ++npivot;
  }

  //if(ostart < opivot && nstart < npivot) {

  //diff_type.value = whitespace;
  //diff_old_start.properties = &diff_type;
  //diff_new_start.properties = &diff_type;

  //}


  if(ostart < opivot) {

    // output delete
    output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

    for(int i = ostart; i < opivot; ++i)
      output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(i), DELETE, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

  }

  if(nstart < npivot) {

    // output insert
    output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

    for(int i = nstart; i < npivot; ++i)
      output_node(rbuf_old, rbuf_new, rbuf_new.nodes.at(i), INSERT, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

  }

  if(opivot < oend) {

  // output common
  output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);

  for(int i = opivot; i < oend; ++i)
    output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(i), COMMON, wstate);

  output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

  }

  rbuf_old.last_output = oend > (signed)rbuf_old.last_output ? oend : rbuf_old.last_output;
  rbuf_new.last_output = nend > (signed)rbuf_new.last_output ? nend : rbuf_new.last_output;


  diff_old_start.properties = 0;
  diff_new_start.properties = 0;

}

void advance_white_space_suffix(reader_state & rbuf_old
                                , int & end_old
                                , reader_state & rbuf_new
                                , int & end_new) {

  int ostart = end_old;
  int nstart = end_new;
  int oend = ostart;
  int nend = nstart;

  // advance all whitespace
  for(; oend < (signed)rbuf_old.nodes.size() && is_white_space(rbuf_old.nodes.at(oend)); ++oend)
    ;

  for(; nend < (signed)rbuf_new.nodes.size() && is_white_space(rbuf_new.nodes.at(nend)); ++nend)
    ;

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart
        && node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) == 0; --opivot, --npivot)
    ;

  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) != 0) {
    ++opivot;
    ++npivot;
  }

  end_old = opivot;
  end_new = npivot;

}

void markup_whitespace(reader_state & rbuf_old, unsigned int end_old, reader_state & rbuf_new, unsigned int end_new, writer_state & wstate) {

  int begin_old = rbuf_old.last_output;
  int begin_new = rbuf_new.last_output;

  int oend = end_old;
  int nend = end_new;

  // set attribute to change
  diff_type.value = whitespace;
  diff_old_start.properties = &diff_type;
  diff_new_start.properties = &diff_type;

  int ostart = begin_old;
  int nstart = begin_new;

  // advance whitespace while matches
  for(; ostart < oend && nstart < nend
        && is_white_space(rbuf_old.nodes.at(ostart)) && is_white_space(rbuf_new.nodes.at(nstart))
        && node_compare(rbuf_old.nodes.at(ostart), rbuf_new.nodes.at(nstart)) == 0; ++ostart, ++nstart)
    ;

  if(begin_old < ostart) {

    output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);
    
    for(int i = begin_old; i < ostart; ++i)
      output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(i), COMMON, wstate);
    
    output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

  }

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart && node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) == 0; --opivot, --npivot)
    ;

  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(node_compare(rbuf_old.nodes.at(opivot), rbuf_new.nodes.at(npivot)) != 0) {

    ++opivot;
    ++npivot;
  }

  if(ostart < opivot) {

    output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

    for(int k = ostart; k < opivot; ++k)
      output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(k), DELETE, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

  }

  if(nstart < npivot) {

    output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

    for(int k = nstart; k < npivot; ++k)
      output_node(rbuf_old, rbuf_new, rbuf_new.nodes.at(k), INSERT, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

  }

  if(opivot < oend) {

    output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);

    for(int k = opivot; k < oend; ++k)
      output_node(rbuf_old, rbuf_new, rbuf_old.nodes.at(k), COMMON, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

  }

    rbuf_old.last_output = oend > (signed)rbuf_old.last_output ? oend : rbuf_old.last_output;
    rbuf_new.last_output = nend > (signed)rbuf_new.last_output ? nend : rbuf_new.last_output;

    diff_old_start.properties = 0;
    diff_new_start.properties = 0;


}
