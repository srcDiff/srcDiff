// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file call.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <call.hpp>

#include <srcdiff_text_measure.hpp>

std::shared_ptr<const name_t> call::name() const {
    std::shared_ptr<const name_t> full_name = named_construct::name();
    if(full_name->root_term()->is_simple()) return full_name;

    return std::static_pointer_cast<const name_t>(full_name->children().back());
}

bool call::is_matchable_impl(const construct & modified) const {
    std::shared_ptr<const name_t> original_name = name();
    std::shared_ptr<const name_t> modified_name = dynamic_cast<const call &>(modified).name();

    srcdiff_text_measure text_measure(*original_name, *modified_name, false);
    text_measure.compute();

    return bool(text_measure.similarity());
}
