// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file convert_differ.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <convert_differ.hpp>

#include <constants.hpp>
#include <change_stream.hpp>
#include <comment_differ.hpp>
#include <common_stream.hpp>
#include <differ.hpp>
#include <whitespace_stream.hpp>
#include <shortest_edit_script.h>

#include <if_stmt.hpp>
#include <if.hpp>

#include <cstring>
#include <list>
#include <algorithm>
#include <iterator>

namespace srcdiff {

const std::string convert_str("convert");
const srcML::attribute diff_convert_type("type", srcML::name_space::SRC_NAMESPACE, convert_str);

convert_differ::convert_differ(std::shared_ptr<output_stream> out, std::shared_ptr<const construct> original_construct, std::shared_ptr<const construct> modified_construct) 
    : out(out), original_construct(original_construct), modified_construct(modified_construct) {}

void convert_differ::output() {

    whitespace_stream whitespace(*out);
    whitespace.output_all();

    /**@todo replace get_descendent with using children above */
    int original_collect_start_pos = 1;
    if(original_construct->root_term_name() == "if_stmt") {
        while(original_construct->term(original_collect_start_pos)->get_name() != "if") {
            ++original_collect_start_pos;
        }
        ++original_collect_start_pos;
    }

    int modified_collect_start_pos = 1;
    if(modified_construct->root_term_name() == "if_stmt") {
        while(modified_construct->term(modified_collect_start_pos)->get_name() != "if") {
          ++modified_collect_start_pos;
        }
      ++modified_collect_start_pos;
    }

    // get keyword if present
    const std::shared_ptr<srcML::node> & keyword_node_original = original_construct->term(original_collect_start_pos);
    const std::shared_ptr<srcML::node> & keyword_node_modified = modified_construct->term(modified_collect_start_pos);

    bool is_keyword  = keyword_node_original->is_text() && !keyword_node_original->is_whitespace();
    bool is_keywords = is_keyword && keyword_node_modified->is_text() && !keyword_node_modified->is_whitespace();
    bool is_same_keyword = is_keywords && *keyword_node_original == *keyword_node_modified;

    if((is_keyword && !is_keywords) || (is_keywords && !is_same_keyword)) {
        ++original_collect_start_pos;
    }

    // output deleted nodes
    out->diff_original_start->emplace_attribute("type", diff_convert_type);
    out->output_node(out->diff_original_start, DELETE, true);
    out->diff_original_start->clear_attributes();

    for(int output_pos = 0; output_pos < original_collect_start_pos; ++output_pos) {
        out->output_node(original_construct->term(output_pos), DELETE);
        ++out->last_output_original();
    }

    // output inserted nodes
    out->diff_modified_start->emplace_attribute("type", diff_convert_type);
    out->output_node(out->diff_modified_start, INSERT, true);
    out->diff_modified_start->clear_attributes();

    if(is_keywords && !is_same_keyword){
        ++modified_collect_start_pos;
    }

    for(int output_pos = 0; output_pos < modified_collect_start_pos; ++output_pos) {
        out->output_node(modified_construct->term(output_pos), INSERT);
        ++out->last_output_modified();
    }

    // collect subset of nodes
    construct::construct_list next_set_original
        = original_construct->get_descendents(original_construct->get_terms().at(original_collect_start_pos),
                                              original_construct->end_position());

    construct::construct_list next_set_modified
        = modified_construct->get_descendents(modified_construct->get_terms().at(modified_collect_start_pos),
                                              modified_construct->end_position());

    differ diff(out, next_set_original, next_set_modified);
    diff.output();

    whitespace_stream::output_whitespace(out);

    change_stream::output_change(out, out->last_output_original(), modified_construct->end_position() + 1);

    out->output_node(out->diff_modified_end, INSERT, true);
    if(out->output_state() == INSERT) {
        out->output_node(out->diff_modified_end, INSERT);
    }

    change_stream::output_change(out, original_construct->end_position() + 1, out->last_output_modified());

    out->output_node(out->diff_original_end, DELETE, true);
    if(out->output_state() == DELETE) {
            out->output_node(out->diff_original_end, DELETE);
    }

}

}
