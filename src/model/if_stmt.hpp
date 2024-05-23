/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_IF_STMT_HPP
#define INCLUDED_IF_STMT_HPP

#include <conditional.hpp>

class if_t;
class else_t;

class if_stmt : public conditional {

public:

    if_stmt(const construct* parent, std::size_t& start);

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
