// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_many.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFFMANY_HPP
#define INCLUDED_SRCDIFFMANY_HPP

#include <shortest_edit_script.h>
#include <differ.hpp>

#include <vector>

class srcdiff_many : public srcdiff::differ {

protected:

    edit_t * edit_script;

private:

    typedef std::pair<int, int> int_pair;
    typedef std::vector<int_pair> int_pairs;
    typedef std::vector<int_pairs> moves;

    void output_unmatched(construct::construct_list_view original_unmatched, construct::construct_list_view modified_unmatched);

    moves determine_operations();
  
public:

    srcdiff_many(const srcdiff::differ & diff, edit_t * edit_script);
    virtual void output();

};

#endif
