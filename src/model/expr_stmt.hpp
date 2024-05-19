/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#ifndef INCLUDED_EXPR_STMT_HPP
#define INCLUDED_EXPR_STMT_HPP

#include <expr_construct.hpp>

class expr_stmt : public expr_construct {

public:
    expr_stmt(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out) : construct(node_list, start, out), expr_construct(node_list, start, out) {}
    virtual bool is_matchable_impl(const construct & modified) const;
};


#endif
