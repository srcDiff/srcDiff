/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_EXPR_CONSTRUCT_HPP
#define INCLUDED_EXPR_CONSTRUCT_HPP

#include <construct.hpp>

#include <expr.hpp>

class expr_construct : virtual public construct {

public:

    expr_construct(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), expr_child() {} 

    virtual std::shared_ptr<const expr_t> expr(const expr_construct & that) const;

    virtual bool is_tag_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;
protected:
    mutable std::optional<std::shared_ptr<const expr_t>> expr_child;
};


#endif
