// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file match_differ.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <match_differ.hpp>

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

match_differ::match_differ(std::shared_ptr<output_stream> out, std::shared_ptr<const construct> original_construct, std::shared_ptr<const construct> modified_construct) 
    : out(out), original_construct(original_construct), modified_construct(modified_construct) {}

void match_differ::output() {

    whitespace_stream whitespace(*out);
    whitespace.output_all();

    if(original_construct->root_term()->is_temporary() == modified_construct->root_term()->is_temporary()) {
        out->output_node(out->diff_common_start, COMMON);
    }

    if(original_construct->root_term()->is_equal(*modified_construct->root_term()), false) {

        out->output_node(original_construct->root_term(), modified_construct->root_term(), COMMON);

    } else {

        std::shared_ptr<srcML::node> merged_node = std::make_shared<srcML::node>(*original_construct->root_term());
        merged_node->merge(*modified_construct->root_term());
        out->output_node(merged_node, COMMON);

    }

    ++out->last_output_original();
    ++out->last_output_modified();

    // diff comments differently then source-code
    if(original_construct->root_term()->get_name() == "comment") {

        // collect subset of nodes
        construct::construct_list children_original = original_construct->children();

        construct::construct_list children_modified = modified_construct->children();

        comment_differ diff(out, children_original, children_modified);
        diff.output();

    } else {

            // collect subset of nodes
            construct::construct_list children_original;
            if(!original_construct->root_term()->is_empty())
                children_original = original_construct->children();

            construct::construct_list children_modified;
            if(!modified_construct->root_term()->is_empty())
                children_modified = modified_construct->children();

            differ diff(out, children_original, children_modified);
            diff.output();

    }

    common_stream::output_common(out, original_construct->end_position() + 1, modified_construct->end_position() + 1);

    if(original_construct->root_term()->is_temporary() == modified_construct->root_term()->is_temporary()) {
        out->output_node(out->diff_common_end, COMMON);
    }

}

}
