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
#include <edit_list.hpp>
#include <change_list.hpp>
#include <operation.hpp>

#include <vector>

namespace srcdiff {

class many_differ : public differ {

protected:

    const struct ses::edit& edit;

    construct::construct_list original_sets;
    construct::construct_list modified_sets;

private:

    void output_unmatched(construct::construct_list_view original_unmatched, construct::construct_list_view modified_unmatched);
    change_list determine_operations();
  
public:

    many_differ(const differ& diff, const struct ses::edit& edit);
    virtual void output();

};

}

#endif
