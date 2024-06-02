/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CLAUSE_HPP
#define INCLUDED_CLAUSE_HPP

#include <conditional.hpp>

#include <unordered_set>

class clause : public conditional {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    clause(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : conditional(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)), block_child() {}
    virtual std::shared_ptr<const construct> block() const;

protected:
    mutable std::optional<std::shared_ptr<const construct>> block_child;
};


#endif
