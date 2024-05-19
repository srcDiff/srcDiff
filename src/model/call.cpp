/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include <call.hpp>

#include <srcdiff_text_measure.hpp>

std::shared_ptr<const construct> call::function_name() const {
    std::shared_ptr<const name_t> full_name = name();
    if(full_name->root_term()->is_simple()) return full_name;

    return full_name->children().back();
}

bool call::is_matchable_impl(const construct & modified) const {
    std::shared_ptr<const construct> original_name = function_name();
    std::shared_ptr<const construct> modified_name = dynamic_cast<const call &>(modified).function_name();

    srcdiff_text_measure text_measure(*original_name, *modified_name, false);
    text_measure.compute();

    return bool(text_measure.similarity());
}
