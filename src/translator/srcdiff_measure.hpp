// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_measure.hpp
 *
 * @copyright Copyright (C) 2012-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_MEASURE_HPP
#define INCLUDED_SRCDIFF_MEASURE_HPP


#include <shortest_edit_script.hpp>

class construct;

class srcdiff_measure {

protected:
    const construct & set_original;
    const construct & set_modified;

    bool computed;

    int a_similarity;
    int a_original_difference;
    int a_modified_difference;
    int original_len;
    int modified_len;

    void process_edit_script(const edit_t * edit_script);

public:

    srcdiff_measure(const construct & set_original, const construct & set_modified);

    int similarity()          const;
    int difference()          const;
    int original_difference() const;
    int modified_difference() const;
    int original_length()     const;
    int modified_length()     const;

    int max_length() const;
    int min_length() const;

    virtual void compute() = 0;
    virtual ~srcdiff_measure() {};
};





#endif
