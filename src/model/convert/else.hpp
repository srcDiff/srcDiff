// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file else.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONVERT_ELSE_HPP
#define INCLUDED_CONVERT_ELSE_HPP

class construct;

#include "rule_checker.hpp"

namespace convert {

class else_t : public rule_checker {

public:

    else_t(const construct& client);

    virtual bool is_tag_convertable(const construct & modified) const;

protected:
};

}

#endif
