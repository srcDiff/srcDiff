// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_change.hpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_CHANGE_HPP
#define INCLUDED_SRCDIFF_CHANGE_HPP

#include <srcdiff_output.hpp>

class srcdiff_change : public srcdiff_output {

protected:
 
    std::size_t end_original;
    std::size_t end_modified;

private:

public:

    static void output_change(std::shared_ptr<srcdiff_output> out, std::size_t end_original, std::size_t end_modified) {
     srcdiff_change change(*out, end_original, end_modified);
     change.output();
    }

    srcdiff_change(const srcdiff_output & out, std::size_t end_original, std::size_t end_modified);

    virtual void output_whitespace_all();
    virtual void output_whitespace_prefix();
    virtual void output();

};

#endif
