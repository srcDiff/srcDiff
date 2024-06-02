/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONDITIONAL_HPP
#define INCLUDED_CONDITIONAL_HPP

#include <construct.hpp>

#include <unordered_set>

class conditional : public construct {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    conditional(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)), condition_child() {
    }

    virtual std::shared_ptr<const construct> condition() const;
    virtual bool is_matchable_impl(const construct & modified) const;
protected:
    mutable std::optional<std::shared_ptr<const construct>> condition_child;

    static const std::unordered_set<std::string>conditional_convertable;
};


#endif
