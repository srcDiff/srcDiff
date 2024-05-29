/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_ACCESS_REGION_HPP
#define INCLUDED_ACCESS_REGION_HPP

#include <always_matched_construct.hpp>

class access_region : public always_matched_construct {

public:

    template<class nest_rule_checker>
    access_region(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker)
        : always_matched_construct(parent, start, std::make_shared<nest_rule_checker>(*this)) {}
    virtual bool is_tag_convertable(const construct & modified) const;
private:
};

#endif
