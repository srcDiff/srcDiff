/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#ifndef INCLUDED_ELSEIF_HPP
#define INCLUDED_ELSEIF_HPP

#include <clause.hpp>
#include <if.hpp>

class elseif : public clause {

public:

    elseif(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : clause(node_list, start, out), if_child() {}

    std::shared_ptr<const if_t> find_if() const;

    virtual std::shared_ptr<const construct> condition() const;
    virtual std::shared_ptr<const construct> block() const;

protected:
    mutable std::optional<std::shared_ptr<const if_t>> if_child;
};


#endif
