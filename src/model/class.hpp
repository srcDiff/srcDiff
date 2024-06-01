/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CLASS_HPP
#define INCLUDED_CLASS_HPP

#include <named_construct.hpp>

class class_t : public named_construct {

public:

    template<class nest_rule_checker, class convert_rule_checker>
    class_t(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker, std::shared_ptr<convert_rule_checker> convert_checker)
        : construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)), named_construct(parent, start, std::make_shared<nest_rule_checker>(*this), std::make_shared<convert_rule_checker>(*this)) {
    }
    virtual bool is_tag_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;

private:

};


#endif
