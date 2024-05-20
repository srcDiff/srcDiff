/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CASE_HPP
#define INCLUDED_CASE_HPP

#include <construct.hpp>

class case_t : public construct {

public:

    case_t(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out) : construct(node_list, start, out) {}
    std::shared_ptr<const construct> expr() const;
    virtual bool is_matchable_impl(const construct & modified) const;
private:
    mutable std::optional<std::shared_ptr<const construct>> expr_child;
};


#endif
