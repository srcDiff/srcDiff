// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file character_diff.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CHARACTER_DIFF_HPP
#define INCLUDED_CHARACTER_DIFF_HPP

#include <shortest_edit_script.hpp>
#include <versioned_string.hpp>

class view_t;

class character_diff {

private:

    const versioned_string& str;

public:

    character_diff(const versioned_string& original);
    void output(view_t& view, const std::string& type);

};


#endif
