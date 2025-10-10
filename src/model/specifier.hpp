// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file specifier.hpp
 *
 * @copyright Copyright (C) 2025-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SPECIFIER_HPP
#define INCLUDED_SPECIFIER_HPP

#include <construct.hpp>

#include <string>

class specifier : public construct {

public:

    specifier(const construct* parent, std::size_t& start)
        : construct(parent, start) {}
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
