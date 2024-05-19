/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_BLOCK_HPP
#define INCLUDED_BLOCK_HPP

#include <construct.hpp>

class block : public construct {

public:

    block(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), block_content_child() {}

    std::shared_ptr<const construct> block_content() const;

    bool is_syntax_similar_impl(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;
private:
    mutable std::optional<std::shared_ptr<const construct>> block_content_child;
};

#endif
