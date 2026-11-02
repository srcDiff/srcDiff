// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file call.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <call.hpp>

#include <text_measurer.hpp>

std::shared_ptr<const name_t> call::name() const {
    std::shared_ptr<const name_t> full_name = named_construct::name();

    // a call of a call has no name. Use the name of the called call
    if(!full_name) {
        std::shared_ptr<const construct> callee = find_child("call");
        return callee ? dynamic_cast<const call &>(*callee).name() : full_name;
    }

    if(full_name->root_term()->is_simple()) return full_name;

    return std::static_pointer_cast<const name_t>(full_name->children().back());
}

bool call::is_matchable_impl(const construct & modified) const {
    std::shared_ptr<const name_t> original_name = name();
    std::shared_ptr<const name_t> modified_name = dynamic_cast<const call &>(modified).name();
    if(!original_name || !modified_name) return !original_name && !modified_name;

    srcdiff::text_measurer text_measure(*original_name, *modified_name, false);
    text_measure.compute();

    return bool(text_measure.similarity());
}
