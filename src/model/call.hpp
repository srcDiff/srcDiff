/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CALL_HPP
#define INCLUDED_CALL_HPP

#include <named_construct.hpp>

class call : public named_construct {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    call(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)), named_construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)) {
    }
    virtual std::shared_ptr<const  name_t> name() const;
    virtual bool is_matchable_impl(const construct & modified) const;

private:
};


#endif
