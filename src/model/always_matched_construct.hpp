// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file always_matched_construct.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_ALWAYS_MATCHED_CONSTRUCT_HPP
#define INCLUDED_ALWAYS_MATCHED_CONSTRUCT_HPP

#include <construct.hpp>

#include <string>

class always_matched_construct : public construct {

public:

    always_matched_construct(const construct* parent, std::size_t& start)
        : construct(parent, start) {}
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
