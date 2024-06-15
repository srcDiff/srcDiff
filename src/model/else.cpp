// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file else.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <else.hpp>

std::shared_ptr<const construct> else_t::condition() const {
   return std::shared_ptr<const construct>();
}
