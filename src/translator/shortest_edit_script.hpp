// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT
#define INCLUDED_SHORTEST_EDIT_SCRIPT

#include <shortest_edit_script_t.hpp>

#include <construct.hpp>
#include <srcml_nodes.hpp>

#include <string>
#include <memory>

namespace srcdiff {

class shortest_edit_script : public ses::shortest_edit_script_t {
public:
    shortest_edit_script() : shortest_edit_script_t(nullptr, nullptr, nullptr) {}

    int compute_edit_script(const std::shared_ptr<construct>& original,    const std::shared_ptr<construct>& modified);
    int compute_edit_script(const construct::construct_list_view original, const construct::construct_list_view modified);
    int compute_edit_script(const std::vector<std::string>& original,      const std::vector<std::string>& modified);
    int compute_edit_script(const std::string& original,                   const std::string& modified);

private:
    static const void* construct_list_index(int idx, const void* s, const void* context);
    static int construct_compare(const void* e1, const void* e2, const void* context);

    static const void* construct_node_index(int idx, const void*s, const void* context);
    static int node_compare(const void* node1, const void* node2, const void* context);
    static int node_compare(const std::shared_ptr<srcML::node>& node1, const std::shared_ptr<srcML::node>& node2);

    static int string_compare(const void* s1, const void* s2, const void* context);
    static const void* string_index(int idx, const void* s, const void* context);

    static int char_compare(const void* c1, const void* c2, const void* context);
    static const void* char_index(int index, const void* s, const void* context);

};

}

#endif
