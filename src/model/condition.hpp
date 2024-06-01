/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONDITION_HPP
#define INCLUDED_CONDITION_HPP

#include <always_matched_construct.hpp>

class condition : public always_matched_construct {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    condition(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : always_matched_construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)) {
    }
    virtual std::string to_string(bool skip_whitespace = false) const;

protected:
};


#endif
