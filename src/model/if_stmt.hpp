/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_IF_STMT_HPP
#define INCLUDED_IF_STMT_HPP

#include <conditional.hpp>

class if_t;
class else_t;

class if_stmt : public conditional {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    if_stmt(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : conditional(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)), if_child(), else_child() {
    }


    std::shared_ptr<const if_t> find_if() const;
    std::shared_ptr<const else_t> find_else() const;
    virtual std::shared_ptr<const construct> condition() const;

    virtual bool is_syntax_similar_impl(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;

protected:
    mutable std::optional<std::shared_ptr<const if_t>> if_child;
    mutable std::optional<std::shared_ptr<const else_t>> else_child;
};


#endif
