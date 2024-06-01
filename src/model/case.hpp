/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CASE_HPP
#define INCLUDED_CASE_HPP

#include <construct.hpp>

class case_t : public construct {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    case_t(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)) {
    }
    std::shared_ptr<const construct> expr() const;
    virtual bool is_matchable_impl(const construct & modified) const;

private:
    mutable std::optional<std::shared_ptr<const construct>> expr_child;
};

#endif
