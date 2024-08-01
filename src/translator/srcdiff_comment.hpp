// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_comment.hpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_COMMENT_HPP
#define INCLUDED_SRCDIFF_COMMENT_HPP

#include <srcdiff_diff.hpp>

class srcdiff_comment : public srcdiff_diff {

protected:

private:

public:

    srcdiff_comment(std::shared_ptr<srcdiff_output> out, const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified);
    virtual void output();

};

#endif

