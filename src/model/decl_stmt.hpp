/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#ifndef INCLUDED_DECL_STMT_HPP
#define INCLUDED_DECL_STMT_HPP

#include <identifier_decl.hpp>
#include <expr_construct.hpp>

class decl_stmt : public identifier_decl, public expr_construct {

public:
    decl_stmt(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), identifier_decl(node_list, start, out), expr_construct(node_list, start, out) {}

    virtual std::shared_ptr<const expr_t> expr(const expr_construct & that) const;
};


#endif
