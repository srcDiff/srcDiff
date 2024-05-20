/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_ALWAYS_MATCHED_CONSTRUCT_HPP
#define INCLUDED_ALWAYS_MATCHED_CONSTRUCT_HPP

#include <named_construct.hpp>

#include <string>

class always_matched_construct : public construct {

public:

    always_matched_construct(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out) {}
    static bool is_always_match(const std::string & construct_name);
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
