// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_change.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_change.hpp>

#include <srcdiff_constants.hpp>
#include <srcdiff_whitespace.hpp>
#include <srcdiff_move.hpp>
#include <shortest_edit_script.h>

#ifdef __MINGW32__
#include <mingw32.hpp>
#endif

#include <cstring>
#include <string>

const std::string replace("replace");
const srcML::attribute diff_type(DIFF_TYPE, srcML::name_space::SRC_NAMESPACE, replace);

srcdiff_change::srcdiff_change(const srcdiff_output & out, std::size_t end_original, std::size_t end_modified)
: srcdiff_output(out), end_original(end_original), end_modified(end_modified) {}

/*

  Output all prepending whitespace as part of a change.

*/
void srcdiff_change::output_whitespace_all() {

  srcdiff_whitespace whitespace(*this);
  whitespace.output_all();

}

/*

  Adds whitespace to a change. Then outputs the change.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after is not a newline,
  it is included and the following nodes are included if they have a new line.

*/
void srcdiff_change::output_whitespace_prefix() {

  srcdiff_whitespace whitespace(*this);
  whitespace.output_prefix();

}

/*

  Outputs a syntactical diff. Beginning whitespace is narrowed and all
  whitespace even if it could be matches is treated as different.

*/
void srcdiff_change::output() {

  std::size_t begin_original = rbuf_original->last_output;
  std::size_t begin_modified = rbuf_modified->last_output;

  bool is_replace = false;
  if(end_original > begin_original && end_modified > begin_modified) {

    // set attribute to change
    // @todo: modify this to use a different constructor
    diff_original_start->emplace_attribute(diff_type.get_name(), diff_type);
    diff_modified_start->emplace_attribute(diff_type.get_name(), diff_type);
    is_replace = true;

    if(is_delay_type(SES_DELETE)) {
    
      output_node(diff_original_end, SES_DELETE, true);
    }

  }

  if(end_original > begin_original) {

    bool first = true;
    for(std::size_t i = begin_original; i < end_original; ++i) {

      // output diff tag begin
      if(first && is_replace) {

        output_node(diff_original_start, SES_DELETE, is_replace);
        first = false;

      }

      if(rbuf_original->nodes.at(i)->get_move()) {

        if(is_replace) {
          diff_original_start->clear_attributes();
        
        }

        srcdiff_move move(*this, i, SES_DELETE);
        move.output();

        if(is_replace) {
          diff_original_start->emplace_attribute(diff_type.get_name(), diff_type);
        }

        continue;

      }

      // output diff tag begin
      if(first) {

        output_node(diff_original_start, SES_DELETE, is_replace);
        first = false;

      }

      if(rbuf_original->nodes.at(i)->is_whitespace()) {

        rbuf_original->last_output = i;
        srcdiff_whitespace whitespace(*this);
        whitespace.output_all(SES_DELETE);
        i = rbuf_original->last_output - 1;
        continue;

      }

      output_node(rbuf_original->nodes.at(i), SES_DELETE);

    }

    rbuf_original->last_output = end_original;

    if(is_replace) {
      srcdiff_whitespace whitespace(*this);
      whitespace.output_suffix(SES_DELETE);
    }

    // output diff tag end
    if(!first) {
      output_node(diff_original_end, SES_DELETE, is_replace);
    
    }

    if(wstate->output_diff.back()->operation == SES_DELETE) {
      output_node(diff_original_end, SES_DELETE);
    
    }

  }

  if(end_modified > begin_modified) {

    bool first = true;
    for(std::size_t i = begin_modified; i < end_modified; ++i) {

      // output diff tag
      if(first && is_replace) {

        output_node(diff_modified_start, SES_INSERT, is_replace);
        first = false;

      }

      if(rbuf_modified->nodes.at(i)->get_move()) {

        if(is_replace) {
          diff_modified_start->clear_attributes();
        
        }

        srcdiff_move move(*this, i, SES_INSERT);
      
        move.output();

        if(is_replace) {
          diff_modified_start->emplace_attribute(diff_type.get_name(), diff_type);
        }

        continue;

      }

      // output diff tag
      if(first) {

        output_node(diff_modified_start, SES_INSERT, is_replace);
        first = false;

      }

      if(rbuf_modified->nodes.at(i)->is_whitespace()) {

        rbuf_modified->last_output = i;
        srcdiff_whitespace whitespace(*this);
        whitespace.output_all(SES_INSERT);
        i = rbuf_modified->last_output - 1;
      
        continue;

      }

      output_node(rbuf_modified->nodes.at(i), SES_INSERT);
    

    }

    rbuf_modified->last_output = end_modified;

    if(is_replace) {
      srcdiff_whitespace whitespace(*this);
      whitespace.output_suffix(SES_INSERT);  
    }

    // output diff tag end
    if(!first) {
      output_node(diff_modified_end, SES_INSERT, is_replace);
    
    }
    
    if(wstate->output_diff.back()->operation == SES_INSERT) {
      output_node(diff_modified_end, SES_INSERT);
    
    }
  

  }

  diff_original_start->clear_attributes();
  diff_modified_start->clear_attributes();

}
