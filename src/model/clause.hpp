/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#ifndef INCLUDED_CLAUSE_HPP
#define INCLUDED_CLAUSE_HPP

#include <conditional.hpp>

#include <unordered_set>

class clause : public conditional {

public:

    clause(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : conditional(node_list, start, out), block_child() {}
    virtual std::shared_ptr<const construct> block() const;
    virtual bool is_tag_convertable(const construct & modified) const;

protected:
    mutable std::optional<std::shared_ptr<const construct>> block_child;
};


#endif
