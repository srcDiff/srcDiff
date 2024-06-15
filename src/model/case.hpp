// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file case.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CASE_HPP
#define INCLUDED_CASE_HPP

#include <construct.hpp>

class case_t : public construct {

public:

    case_t(const construct* parent, std::size_t& start)
        : construct(parent, start) {
    }
    std::shared_ptr<const construct> expr() const;
    virtual bool is_matchable_impl(const construct & modified) const;

private:
    mutable std::optional<std::shared_ptr<const construct>> expr_child;
};

#endif
