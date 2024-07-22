// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file for.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_FOR_T_HPP
#define INCLUDED_FOR_T_HPP

#include <conditional.hpp>

#include <unordered_set>

class for_t : public conditional {

public:

    for_t(const construct* parent, std::size_t& start)
        : conditional(parent, start), control_child() {
    }

    std::shared_ptr<const construct> control() const;
    virtual std::shared_ptr<const construct> condition() const;
    virtual bool is_matchable_impl(const construct & modified) const;
protected:
    mutable std::optional<std::shared_ptr<const construct>> control_child;
};


#endif
