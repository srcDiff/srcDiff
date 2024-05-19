/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_IF_HPP
#define INCLUDED_IF_HPP

#include <clause.hpp>

class if_t : public clause {

public:

    if_t(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : clause(node_list, start, out) {}

    bool has_real_block() const;
    bool is_block_matchable(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;

protected:
};


#endif
