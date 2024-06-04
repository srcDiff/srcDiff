/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_NAMED_CONSTRUCT_HPP
#define INCLUDED_NAMED_CONSTRUCT_HPP

#include <construct.hpp>

#include <name.hpp>

class named_construct : virtual public construct {

public:

    named_construct(const construct* parent, std::size_t& start)
        : construct(parent, start), name_child() {} 

    virtual std::shared_ptr<const name_t> name() const;
    virtual bool is_matchable_impl(const construct & modified) const;

protected:
    mutable std::optional<std::shared_ptr<const name_t>> name_child;
};


#endif
