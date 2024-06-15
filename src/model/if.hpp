// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file if.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_IF_HPP
#define INCLUDED_IF_HPP

#include <clause.hpp>

class if_t : public clause {

public:

    if_t(const construct* parent, std::size_t& start)
        : clause(parent, start) {
    }

    bool has_real_block() const;
    bool is_block_matchable(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;

protected:
};


#endif
