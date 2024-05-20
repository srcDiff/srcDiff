/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <identifier_decl.hpp>

#include <srcdiff_text_measure.hpp>

std::shared_ptr<const name_t> identifier_decl::name() const {
    if(name_child) return *name_child;

    name_child = std::shared_ptr<const name_t>();
    std::shared_ptr<const construct> decl_child = find_child("decl");
    if(decl_child) {
        name_child = dynamic_cast<const named_construct &>(*decl_child).name();
    }

    return *name_child;
}

bool identifier_decl::is_matchable_impl(const construct & modified) const {
    std::shared_ptr<const name_t> original_name = name();
    std::shared_ptr<const name_t> modified_name = dynamic_cast<const named_construct &>(modified).name();

    if(!original_name || !modified_name) return false;

    return original_name->to_string() == modified_name->to_string();
}
