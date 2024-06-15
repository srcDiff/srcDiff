// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file conditional.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONDITIONAL_HPP
#define INCLUDED_CONDITIONAL_HPP

#include <construct.hpp>

#include <unordered_set>

class conditional : public construct {

public:

    conditional(const construct* parent, std::size_t& start)
        : construct(parent, start), condition_child() {
    }

    virtual std::shared_ptr<const construct> condition() const;
    virtual bool is_matchable_impl(const construct & modified) const;
protected:
    mutable std::optional<std::shared_ptr<const construct>> condition_child;

    static const std::unordered_set<std::string>conditional_convertable;
};


#endif
