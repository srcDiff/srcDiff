// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file name.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_NEST_NAME_HPP
#define INCLUDED_NEST_NAME_HPP

#include "rule_checker.hpp"

namespace nest {

class name_t : public rule_checker {

public:
    name_t(const construct& client);

    bool check_nest_name(const construct & modified, bool find_name_parent = true) const;
    virtual bool check_nest(const construct & modified) const;

private:
};

}

#endif
