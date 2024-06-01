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

    template<class nest_rule_checker, class convert_rule_checker>
    named_construct(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)), name_child() {} 

    virtual std::shared_ptr<const name_t> name() const;
    virtual bool is_matchable_impl(const construct & modified) const;

protected:
    mutable std::optional<std::shared_ptr<const name_t>> name_child;
};


#endif
