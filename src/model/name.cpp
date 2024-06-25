/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#include <name.hpp>

bool name_t::is_matchable_impl(const construct & modified) const {

    if(root_term()->is_simple() && modified.root_term()->is_simple()) return true;

    return false;
}
