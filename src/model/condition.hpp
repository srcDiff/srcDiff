// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file condition.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONDITION_HPP
#define INCLUDED_CONDITION_HPP

#include <always_match.hpp>

class condition : public always_match {

public:

    condition(const construct* parent, std::size_t& start)
        : always_match(parent, start) {
    }
    virtual std::string to_string(bool skip_whitespace = false) const;

protected:
};


#endif
