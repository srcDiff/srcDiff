// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file if_stmt.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_IF_STMT_HPP
#define INCLUDED_IF_STMT_HPP

#include <conditional.hpp>
#include <if.hpp>
#include <else.hpp>

class if_stmt : public conditional {

public:

    if_stmt(const construct* parent, std::size_t& start)
        : conditional(parent, start), if_child(), else_child() {
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
