// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_whitespace.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_whitespace.hpp>

#include <srcdiff_common.hpp>
#include <type_query.hpp>
#include <shortest_edit_script.h>

srcdiff_whitespace::srcdiff_whitespace(const srcdiff::output_stream & out) : srcdiff::output_stream(out) {}

void srcdiff_whitespace::markup_whitespace(unsigned int end_original, unsigned int end_modified) {

  int begin_original = rbuf_original->last_output;
  int begin_modified = rbuf_modified->last_output;

  int oend = end_original;
  int nend = end_modified;

  int ostart = begin_original;
  int nstart = begin_modified;

  // advance whitespace while matches
  for(; ostart < oend && nstart < nend
        && rbuf_original->nodes.at(ostart)->is_whitespace() && rbuf_modified->nodes.at(nstart)->is_whitespace()
        && *rbuf_original->nodes.at(ostart) == *rbuf_modified->nodes.at(nstart); ++ostart, ++nstart)
    ;

  if(begin_original < ostart) {

    output_node(diff_common_start, SES_COMMON);
    
    for(int i = begin_original; i < ostart; ++i) {
      output_node(rbuf_original->nodes.at(i), SES_COMMON);
    }
    
    output_node(diff_common_end, SES_COMMON);

  }

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart && *rbuf_original->nodes.at(opivot) == *rbuf_modified->nodes.at(npivot); --opivot, --npivot)
    ;

  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(*rbuf_original->nodes.at(opivot) != *rbuf_modified->nodes.at(npivot)) {

    ++opivot;
    ++npivot;
  }

  if(wstate->output_diff.back()->operation == SES_INSERT) {

    if(nstart < npivot) {

      output_node(diff_modified_start, SES_INSERT);
      output_node(diff_ws_start, SES_INSERT);

      for(int k = nstart; k < npivot; ++k) {
        output_node(rbuf_modified->nodes.at(k), SES_INSERT);
      }

      output_node(diff_ws_end, SES_INSERT);
      output_node(diff_modified_end, SES_INSERT);

    }

    if(ostart < opivot) {

      output_node(diff_original_start, SES_DELETE);
      output_node(diff_ws_start, SES_DELETE);

      for(int k = ostart; k < opivot; ++k) {
        output_node(rbuf_original->nodes.at(k), SES_DELETE);
      }

      output_node(diff_ws_end, SES_DELETE);
      output_node(diff_original_end, SES_DELETE);

    }

  } else {

    if(ostart < opivot) {

      output_node(diff_original_start, SES_DELETE);
      output_node(diff_ws_start, SES_DELETE);

      for(int k = ostart; k < opivot; ++k) {
        output_node(rbuf_original->nodes.at(k), SES_DELETE);
      }

      output_node(diff_ws_end, SES_DELETE);
      output_node(diff_original_end, SES_DELETE);

    }

    if(nstart < npivot) {

      output_node(diff_modified_start, SES_INSERT);
      output_node(diff_ws_start, SES_INSERT);

      for(int k = nstart; k < npivot; ++k) {
        output_node(rbuf_modified->nodes.at(k), SES_INSERT);
      }

      output_node(diff_ws_end, SES_INSERT);
      output_node(diff_modified_end, SES_INSERT);

    }

  }

  if(opivot < oend) {

    output_node(diff_common_start, SES_COMMON);

    for(int k = opivot; k < oend; ++k) {
      output_node(rbuf_original->nodes.at(k), SES_COMMON);
    }

    // output diff tag
    output_node(diff_common_end, SES_COMMON);

  }

    rbuf_original->last_output = oend > (signed)rbuf_original->last_output ? oend : rbuf_original->last_output;
    rbuf_modified->last_output = nend > (signed)rbuf_modified->last_output ? nend : rbuf_modified->last_output;

}

int srcdiff_whitespace::extend_end_to_new_line(std::shared_ptr<reader_state> rbuf) {

  unsigned int end = rbuf->last_output;

  for(; end < rbuf->nodes.size() && rbuf->nodes.at(end)->is_whitespace() && !rbuf->nodes.at(end)->is_new_line(); ++end)
    ;

  if(end < rbuf->nodes.size() && rbuf->nodes.at(end)->is_new_line()) {
    ++end;
  }

  return end;
}


void srcdiff_whitespace::output_nested(int operation) {

  unsigned int oend = rbuf_original->last_output;
  unsigned int nend = rbuf_modified->last_output;

  if(operation == SES_DELETE) {
    for(; oend < rbuf_original->nodes.size() && rbuf_original->nodes.at(oend)->is_whitespace(); ++oend) 
      ;
  }
  else {
    for(; nend < rbuf_modified->nodes.size() && rbuf_modified->nodes.at(nend)->is_whitespace(); ++nend)
      ;
  }
  markup_whitespace(oend, nend);

}

void srcdiff_whitespace::output_statement() {

  unsigned int oend = rbuf_original->last_output;
  unsigned int nend = rbuf_modified->last_output;

  if(oend >= 1 && !is_statement(rbuf_original->nodes.at(oend - 1)->get_name())
    && nend >= 1 && !is_statement(rbuf_modified->nodes.at(nend - 1)->get_name())) return;

  // advance whitespace after targeted end
  oend = extend_end_to_new_line(rbuf_original);
  nend = extend_end_to_new_line(rbuf_modified);

  markup_whitespace(oend, nend);

}

void srcdiff_whitespace::output_all(int operation) {

  unsigned int oend = rbuf_original->last_output;
  unsigned int nend = rbuf_modified->last_output;

  // advance whitespace after targeted end
  if(operation == SES_COMMON || operation == SES_DELETE) {
    for(; oend < rbuf_original->nodes.size() && rbuf_original->nodes.at(oend)->is_whitespace(); ++oend)
    ;

  }

  if(operation == SES_COMMON || operation == SES_INSERT) {
    for(; nend < rbuf_modified->nodes.size() && rbuf_modified->nodes.at(nend)->is_whitespace(); ++nend)
    ;
  }

  markup_whitespace(oend, nend);

}

void srcdiff_whitespace::output_prefix() {

  unsigned int ostart = rbuf_original->last_output;
  unsigned int nstart = rbuf_modified->last_output;
  unsigned int oend = ostart;
  unsigned int nend = nstart;

  // advance whitespace while matches
  for(; oend < rbuf_original->nodes.size() && nend < rbuf_modified->nodes.size()
        && rbuf_original->nodes.at(oend)->is_whitespace() && rbuf_modified->nodes.at(nend)->is_whitespace()
        && *rbuf_original->nodes.at(oend) == *rbuf_modified->nodes.at(nend); ++oend, ++nend)
    ;
  /*

    Change back to if's when not collecting whitespace as single nodes

  */
  // may only match here, but belongs as part of pure change
  unsigned int save_oend = oend;
  unsigned int save_nend = nend;

  while(rbuf_original->last_output < oend && (rbuf_original->nodes.at(oend - 1)->is_whitespace() && !rbuf_original->nodes.at(oend - 1)->is_new_line())) {
    --oend;
  }

  while(rbuf_modified->last_output < nend && (rbuf_modified->nodes.at(nend - 1)->is_whitespace() && !rbuf_modified->nodes.at(nend - 1)->is_new_line())) {
    --nend;
  }

  if(oend > 0 && !rbuf_original->nodes.at(oend - 1)->is_new_line()) {
    oend = save_oend;
  }

  if(nend > 0 && !rbuf_modified->nodes.at(nend - 1)->is_new_line()) {
    nend = save_nend;
  }

  if(ostart < oend && nstart < nend) {

    output_node(diff_common_start, SES_COMMON);

    for(unsigned int i = ostart; i < oend; ++i) {
      output_node(rbuf_original->nodes.at(i), SES_COMMON);
    }

    output_node(diff_common_end, SES_COMMON);

    rbuf_original->last_output = oend > rbuf_original->last_output ? oend : rbuf_original->last_output;
    rbuf_modified->last_output = nend > rbuf_modified->last_output ? nend : rbuf_modified->last_output;

  }

}

void srcdiff_whitespace::output_suffix() {

  int ostart = rbuf_original->last_output;
  int nstart = rbuf_modified->last_output;
  int oend = ostart;
  int nend = nstart;

  // advance all whitespace
  for(; oend < (signed)rbuf_original->nodes.size() && rbuf_original->nodes.at(oend)->is_whitespace(); ++oend)
    ;

  for(; nend < (signed)rbuf_modified->nodes.size() && rbuf_modified->nodes.at(nend)->is_whitespace(); ++nend)
    ;

  int opivot = oend - 1;
  int npivot = nend - 1;

  for(; opivot > ostart && npivot > nstart
        && *rbuf_original->nodes.at(opivot) == *rbuf_modified->nodes.at(npivot); --opivot, --npivot)
    ;


  if(opivot < ostart || npivot < nstart) {

    opivot = oend;
    npivot = nend;

  } else if(*rbuf_original->nodes.at(opivot) != *rbuf_modified->nodes.at(npivot)) {
    ++opivot;
    ++npivot;
  }

  if(ostart < opivot) {

    // output delete
    output_node(diff_original_start, SES_DELETE);

    for(int i = ostart; i < opivot; ++i) {
      output_node(rbuf_original->nodes.at(i), SES_DELETE);
    }

    // output diff tag begin
    output_node(diff_original_end, SES_DELETE);

  }

  if(nstart < npivot) {

    // output insert
    output_node(diff_modified_start, SES_INSERT);

    for(int i = nstart; i < npivot; ++i) {
      output_node(rbuf_modified->nodes.at(i), SES_INSERT);
    }

    // output diff tag begin
    output_node(diff_modified_end, SES_INSERT);

  }

  if(opivot < oend) {

  // output common
  output_node(diff_common_start, SES_COMMON);

  for(int i = opivot; i < oend; ++i) {
    output_node(rbuf_original->nodes.at(i), SES_COMMON);
  }

  output_node(diff_common_end, SES_COMMON);

  }

  rbuf_original->last_output = oend > (signed)rbuf_original->last_output ? oend : rbuf_original->last_output;
  rbuf_modified->last_output = nend > (signed)rbuf_modified->last_output ? nend : rbuf_modified->last_output;

}
