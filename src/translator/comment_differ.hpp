// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file comment_differ.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_COMMENT_DIFFER_HPP
#define INCLUDED_COMMENT_DIFFER_HPP

#include <differ.hpp>

namespace srcdiff {

class comment_differ : public srcdiff::differ {

protected:

private:

public:

    comment_differ(std::shared_ptr<srcdiff::output_stream> out, const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified);
    virtual void output();

};

}

#endif

