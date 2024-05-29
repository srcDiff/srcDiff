/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */

#ifndef INCLUDED_NAME_HPP
#define INCLUDED_NAME_HPP

#include <construct.hpp>

class name_t : public construct {

public:
    // template<class nest_rule_checker>
    name_t(const construct* parent, std::size_t& start/*, std::shared_ptr<nest_rule_checker> nest_checker*/)
        : construct(parent, start/*, std::make_shared<nest_rule_checker>(*this)*/) {
    }

    std::string simple_name() const;
    bool check_nest(const construct & modified, bool find_name_parent = true) const;
    virtual bool can_nest(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;

};


#endif
