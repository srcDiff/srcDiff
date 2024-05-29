/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CAST_HPP
#define INCLUDED_CAST_HPP

#include <named_construct.hpp>

class cast : public construct {

public:

    template<class nest_rule_checker>
    cast(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this)) {}
    virtual bool is_tag_convertable(const construct & modified) const;

private:

};


#endif
