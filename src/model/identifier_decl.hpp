/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_IDENTIFIER_DECL_HPP
#define INCLUDED_IDENTIFIER_DECL_HPP

#include <named_construct.hpp>

class identifier_decl : public named_construct {

public:

    identifier_decl(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), named_construct(node_list, start, out) {}
    virtual std::shared_ptr<const name_t> name() const;
    virtual bool is_matchable_impl(const construct & modified) const;

private:
    
};


#endif
