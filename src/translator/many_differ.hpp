// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file many_differ.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_MANY_DIFFER_HPP
#define INCLUDED_MANY_DIFFER_HPP

#include <shortest_edit_script.h>
#include <differ.hpp>
#include <operation.hpp>

#include <vector>

namespace srcdiff {

class many_differ : public differ {

protected:

    edit_t * edit_script;

private:

    typedef std::pair<operation, int> int_pair;
    typedef std::vector<int_pair> int_pairs;
    typedef std::vector<int_pairs> moves;

    void output_unmatched(construct::construct_list_view original_unmatched, construct::construct_list_view modified_unmatched);

    moves determine_operations();
  
public:

    many_differ(const differ & diff, edit_t * edit_script);
    virtual void output();

};

}

#endif
