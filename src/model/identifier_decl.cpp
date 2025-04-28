// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file identifier_decl.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <identifier_decl.hpp>

#include <text_measurer.hpp>

std::shared_ptr<const name_t> identifier_decl::name() const {
    if(name_child) return *name_child;

    name_child = std::shared_ptr<const name_t>();
    std::shared_ptr<const construct> decl_child = find_child("decl");
    if(decl_child) {
        name_child = dynamic_cast<const named_construct &>(*decl_child).name();
    }

    return *name_child;
}
