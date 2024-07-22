// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file name.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */
 
#ifndef INCLUDED_NAME_HPP
#define INCLUDED_NAME_HPP

#include <construct.hpp>

class name_t : public construct {

public:

    name_t(const construct* parent, std::size_t& start)
        : construct(parent, start) {
    }

    virtual bool is_matchable_impl(const construct & modified) const;

};


#endif
