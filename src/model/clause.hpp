// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file clause.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CLAUSE_HPP
#define INCLUDED_CLAUSE_HPP

#include <conditional.hpp>

#include <unordered_set>

class clause : public conditional {

public:

    clause(const construct* parent, std::size_t& start)
        : conditional(parent, start), block_child() {
    }

    virtual std::shared_ptr<const construct> block() const;

protected:
    mutable std::optional<std::shared_ptr<const construct>> block_child;
};


#endif
