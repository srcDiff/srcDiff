// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file similar.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include "similar.hpp"

#include <construct.hpp>

namespace nest {

    bool similar::check_nest(const construct & modified) const {
        return client.is_similar(modified); 
    }

}
