// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file class.hpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONVERT_CLASS_HPP
#define INCLUDED_CONVERT_CLASS_HPP

class construct;

#include "rule_checker.hpp"

namespace convert {

class class_t : public rule_checker {

public:

    class_t(const construct& client);

    virtual bool is_convertable_impl(const construct & modified) const;

protected:
};

}

#endif
