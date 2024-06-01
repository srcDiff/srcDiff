/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_ELSE_HPP
#define INCLUDED_ELSE_HPP

#include <clause.hpp>

#include <unordered_set>

class else_t : public clause {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    else_t(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : clause(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)) {
    }
    std::shared_ptr<const construct> condition() const;

protected:
};


#endif
