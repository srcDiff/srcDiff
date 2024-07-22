// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file elseif.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <elseif.hpp>

std::shared_ptr<const if_t> elseif::find_if() const {
   if(if_child) return *if_child;

    if_child = std::static_pointer_cast<const if_t>(find_child("if"));
    return *if_child; 
}

std::shared_ptr<const construct> elseif::condition() const {
    return find_if()->condition();
}

std::shared_ptr<const construct> elseif::block() const {
    return static_cast<const if_t &>(*find_if()).block();
}
