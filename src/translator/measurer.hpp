// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file measurer.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_MEASURER_HPP
#define INCLUDED_MEASURER_HPP

#include <shortest_edit_script.hpp>

class construct;

namespace srcdiff {

class measurer {

protected:
    const construct& set_original;
    const construct& set_modified;

    bool computed;

    int a_similarity;
    int a_original_difference;
    int a_modified_difference;
    int original_len;
    int modified_len;

    void process_edit_script(const ses::edit_list& edits);

public:

    measurer(const construct& set_original, const construct& set_modified);

    int similarity()          const;
    int difference()          const;
    int original_difference() const;
    int modified_difference() const;
    int original_length()     const;
    int modified_length()     const;

    int max_length() const;
    int min_length() const;

    virtual void compute() = 0;
    virtual ~measurer() {};
};

}

#endif
