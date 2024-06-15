// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file else.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_ELSE_HPP
#define INCLUDED_ELSE_HPP

#include <clause.hpp>

#include <unordered_set>

class else_t : public clause {

public:

    else_t(const construct* parent, std::size_t& start)
        : clause(parent, start) {
    }
    std::shared_ptr<const construct> condition() const;

protected:
};


#endif
