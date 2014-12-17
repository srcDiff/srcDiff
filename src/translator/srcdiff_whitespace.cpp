#include <srcdiff_whiteSpace.hpp>

#include <srcdiff_common.hpp>

#include <shortest_edit_script.h>
#include <srcDiffConstants.hpp>
#include <srcDiffUtility.hpp>

srcdiff_whitespace::srcdiff_whitespace(const srcdiff_output & out) : srcdiff_output(out) {}

void srcdiff_whitespace::markup_whitespace(unsigned int end_old, unsigned int end_new) {

  int begin_old = rbuf_old->last_output;
  int begin_new = rbuf_new->last_output;

  int oend = end_old;
  int nend = end_new;

  // set attribute to change
  diff_type->value = whitespace;
  diff_old_start->properties = diff_type.get();
  diff_new_start->properties = diff_type.get();

  int ostart = begin_old;
  int nstart = begin_new;

  // advance whitespace while matches
  for(; ostart < oend && nstart < nend
        && is_white_space(rbuf_old->nodes.at(ostart)) && is_white_space(rbuf_new->nodes.at(nstart))
        && node_compare(rbuf_old->nodes.at(ostart), rbuf_new->nodes.at(nstart)) == 0; ++ostart, ++nstart)
    ;

  if(begin_old < ostart) {

    output_node(diff_common_start.get(), SESCOMMON);
    
    for(int i = begin_old; i < ostart; ++i)
      output_node(rbuf_old->nodes.at(i), SESCOMMON);
    
    output_node(diff_common_end.get(), SESCOMMON);

  }

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart && node_compare(rbuf_old->nodes.at(opivot), rbuf_new->nodes.at(npivot)) == 0; --opivot, --npivot)
    ;

  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(node_compare(rbuf_old->nodes.at(opivot), rbuf_new->nodes.at(npivot)) != 0) {

    ++opivot;
    ++npivot;
  }

  if(wstate->output_diff.back()->operation == SESINSERT) {

    if(nstart < npivot) {

      output_node(diff_new_start.get(), SESINSERT);

      for(int k = nstart; k < npivot; ++k)
        output_node(rbuf_new->nodes.at(k), SESINSERT);

      // output diff tag
      output_node(diff_new_end.get(), SESINSERT);

    }

    if(ostart < opivot) {

      output_node(diff_old_start.get(), SESDELETE);

      for(int k = ostart; k < opivot; ++k)
        output_node(rbuf_old->nodes.at(k), SESDELETE);

      // output diff tag
      output_node(diff_old_end.get(), SESDELETE);

    }

  } else {

    if(ostart < opivot) {

      output_node(diff_old_start.get(), SESDELETE);

      for(int k = ostart; k < opivot; ++k)
        output_node(rbuf_old->nodes.at(k), SESDELETE);

      // output diff tag
      output_node(diff_old_end.get(), SESDELETE);

    }

    if(nstart < npivot) {

      output_node(diff_new_start.get(), SESINSERT);

      for(int k = nstart; k < npivot; ++k)
        output_node(rbuf_new->nodes.at(k), SESINSERT);

      // output diff tag
      output_node(diff_new_end.get(), SESINSERT);

    }

  }

  if(opivot < oend) {

    output_node(diff_common_start.get(), SESCOMMON);

    for(int k = opivot; k < oend; ++k)
      output_node(rbuf_old->nodes.at(k), SESCOMMON);

    // output diff tag
    output_node(diff_common_end.get(), SESCOMMON);

  }

    rbuf_old->last_output = oend > (signed)rbuf_old->last_output ? oend : rbuf_old->last_output;
    rbuf_new->last_output = nend > (signed)rbuf_new->last_output ? nend : rbuf_new->last_output;

    diff_old_start->properties = 0;
    diff_new_start->properties = 0;


}

void srcdiff_whitespace::output_white_space_nested(int operation) {

  unsigned int oend = rbuf_old->last_output;
  unsigned int nend = rbuf_new->last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old->nodes.size() && nend < rbuf_new->nodes.size()
        && is_white_space(rbuf_old->nodes.at(oend)) && is_white_space(rbuf_new->nodes.at(nend))
        ; ++oend)
    ;

  if(operation == SESDELETE)
    for(; oend < rbuf_old->nodes.size() && is_white_space(rbuf_old->nodes.at(oend)); ++oend) {
    }
  else
    for(; nend < rbuf_new->nodes.size() && is_white_space(rbuf_new->nodes.at(nend)); ++nend) {
    }

  markup_whitespace(oend, nend);

}


void srcdiff_whitespace::output_white_space_statement() {

  unsigned int oend = rbuf_old->last_output;
  unsigned int nend = rbuf_new->last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old->nodes.size() && is_white_space(rbuf_old->nodes.at(oend)) && !is_new_line(rbuf_old->nodes.at(oend)); ++oend)
    ;

  for(; nend < rbuf_new->nodes.size() && is_white_space(rbuf_new->nodes.at(nend)) && !is_new_line(rbuf_new->nodes.at(nend)); ++nend)
    ;

  if(oend < rbuf_old->nodes.size() && is_new_line(rbuf_old->nodes.at(oend)))
    ++oend;

  if(nend < rbuf_new->nodes.size() && is_new_line(rbuf_new->nodes.at(nend)))
    ++nend;

  markup_whitespace(oend, nend);

}

void srcdiff_whitespace::output_white_space_all() {

  unsigned int oend = rbuf_old->last_output;
  unsigned int nend = rbuf_new->last_output;

  // advance whitespace after targeted end
  for(; oend < rbuf_old->nodes.size() && is_white_space(rbuf_old->nodes.at(oend)); ++oend)
    ;

  for(; nend < rbuf_new->nodes.size() && is_white_space(rbuf_new->nodes.at(nend)); ++nend)
    ;

  markup_whitespace(oend, nend);

}

void srcdiff_whitespace::output_white_space_prefix() {

  unsigned int ostart = rbuf_old->last_output;
  unsigned int nstart = rbuf_new->last_output;
  unsigned int oend = ostart;
  unsigned int nend = nstart;

  // advance whitespace while matches
  for(; oend < rbuf_old->nodes.size() && nend < rbuf_new->nodes.size()
        && is_white_space(rbuf_old->nodes.at(oend)) && is_white_space(rbuf_new->nodes.at(nend))
        && node_compare(rbuf_old->nodes.at(oend), rbuf_new->nodes.at(nend)) == 0; ++oend, ++nend)
    ;
  /*

    Change back to if's when not collecting whitespace as single nodes

  */
  // may only match here, but belongs as part of pure change
  unsigned int save_oend = oend;
  unsigned int save_nend = nend;

  while(rbuf_old->last_output < oend && (is_white_space(rbuf_old->nodes.at(oend - 1)) && !is_new_line(rbuf_old->nodes.at(oend - 1))))
    --oend;

  while(rbuf_new->last_output < nend && (is_white_space(rbuf_new->nodes.at(nend - 1)) && !is_new_line(rbuf_new->nodes.at(nend - 1))))
    --nend;

  if(oend > 0 && !is_new_line(rbuf_old->nodes.at(oend - 1)))
    oend = save_oend;

  if(nend > 0 && !is_new_line(rbuf_new->nodes.at(nend - 1)))
    nend = save_nend;

  if(ostart < oend) {

  output_node(diff_common_start.get(), SESCOMMON);

  for(unsigned int i = ostart; i < oend; ++i)
    output_node(rbuf_old->nodes.at(i), SESCOMMON);

  output_node(diff_common_end.get(), SESCOMMON);

  }

  rbuf_old->last_output = oend > rbuf_old->last_output ? oend : rbuf_old->last_output;
  rbuf_new->last_output = nend > rbuf_new->last_output ? nend : rbuf_new->last_output;

}

void srcdiff_whitespace::output_white_space_suffix() {

  int ostart = rbuf_old->last_output;
  int nstart = rbuf_new->last_output;
  int oend = ostart;
  int nend = nstart;

  // advance all whitespace
  for(; oend < (signed)rbuf_old->nodes.size() && is_white_space(rbuf_old->nodes.at(oend)); ++oend)
    ;

  for(; nend < (signed)rbuf_new->nodes.size() && is_white_space(rbuf_new->nodes.at(nend)); ++nend)
    ;

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart
        && node_compare(rbuf_old->nodes.at(opivot), rbuf_new->nodes.at(npivot)) == 0; --opivot, --npivot)
    ;


  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(node_compare(rbuf_old->nodes.at(opivot), rbuf_new->nodes.at(npivot)) != 0) {
    ++opivot;
    ++npivot;
  }

  //if(ostart < opivot && nstart < npivot) {

  //diff_type.value = whitespace;
  //diff_old_start->properties = &diff_type;
  //diff_new_start->properties = &diff_type;

  //}


  if(ostart < opivot) {

    // output delete
    output_node(diff_old_start.get(), SESDELETE);

    for(int i = ostart; i < opivot; ++i)
      output_node(rbuf_old->nodes.at(i), SESDELETE);

    // output diff tag begin
    output_node(diff_old_end.get(), SESDELETE);

  }

  if(nstart < npivot) {

    // output insert
    output_node(diff_new_start.get(), SESINSERT);

    for(int i = nstart; i < npivot; ++i)
      output_node(rbuf_new->nodes.at(i), SESINSERT);

    // output diff tag begin
    output_node(diff_new_end.get(), SESINSERT);

  }

  if(opivot < oend) {

  // output common
  output_node(diff_common_start.get(), SESCOMMON);

  for(int i = opivot; i < oend; ++i)
    output_node(rbuf_old->nodes.at(i), SESCOMMON);

  output_node(diff_common_end.get(), SESCOMMON);

  }

  rbuf_old->last_output = oend > (signed)rbuf_old->last_output ? oend : rbuf_old->last_output;
  rbuf_new->last_output = nend > (signed)rbuf_new->last_output ? nend : rbuf_new->last_output;


  diff_old_start->properties = 0;
  diff_new_start->properties = 0;

}
