/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CALL_HPP
#define INCLUDED_CALL_HPP

#include <named_construct.hpp>

class call : public named_construct {

public:

    call(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), named_construct(node_list, start, out) {}
    std::shared_ptr<const construct> function_name() const;
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
