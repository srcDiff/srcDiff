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

    call(const construct* parent, std::size_t& start)
        : construct(parent, start), named_construct(parent, start) {}
    virtual std::shared_ptr<const  name_t> name() const;
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
