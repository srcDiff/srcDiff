// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file expr.cpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <expr.hpp>

bool expr_t::is_single_call() const {
    if(children().size() != 1) return false;
    return children().front()->root_term_name() == "call";
}

bool expr_t::is_matchable_impl(const construct & modified) const {
    const srcdiff::measure & expr_measure = *measure(modified);
    if(expr_measure.similarity() > 0 && expr_measure.difference() <= expr_measure.max_length()) return true;

    if(root_term()->get_parent()->get_name() == "argument") return true;
    if(modified.root_term()->get_parent()->get_name() == "argument") return true;

    return children().size() == 1 && children()[0]->root_term_name() == "name";
}
