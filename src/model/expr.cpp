/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#include <expr.hpp>

bool expr_t::is_single_call() const {
    if(children().size() != 1) return false;
    return children().front()->root_term_name() == "call";
}

bool expr_t::can_nest(const construct & modified) const {

    if(parent_term_name() == "init"     && parent_term()->get_parent()->get_parent()->get_name()               == "decl_stmt") return true;
    if(parent_term_name() == "argument" && parent_term()->get_parent()->get_parent()->get_parent()->get_name() == "decl_stmt") return true;

    if(modified.parent_term_name() == "init"     && modified.parent_term()->get_parent()->get_parent()->get_name()               == "decl_stmt") return true;
    if(modified.parent_term_name() == "argument" && modified.parent_term()->get_parent()->get_parent()->get_parent()->get_name() == "decl_stmt") return true;

    return is_similar(modified);
}

bool expr_t::is_matchable_impl(const construct & modified) const {
    const srcdiff_measure & expr_measure = *measure(modified);
    if(expr_measure.similarity() > 0 && expr_measure.difference() <= expr_measure.max_length()) return true;

    if(root_term()->get_parent()->get_name() == "argument") return true;
    if(modified.root_term()->get_parent()->get_name() == "argument") return true;

    return children().size() == 1 && children()[0]->root_term_name() == "name";
}
