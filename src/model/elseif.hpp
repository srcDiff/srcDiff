/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * Copyright (C) 2023-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_ELSEIF_HPP
#define INCLUDED_ELSEIF_HPP

#include <clause.hpp>
#include <if.hpp>

class elseif : public clause {

public:

    template<class nest_rule_checker>
    elseif(const construct* parent, std::size_t& start, std::shared_ptr<nest_rule_checker> nest_checker)
        : clause(parent, start, std::make_shared<nest_rule_checker>(*this)), if_child() {
    }


    std::shared_ptr<const if_t> find_if() const;

    virtual std::shared_ptr<const construct> condition() const;
    virtual std::shared_ptr<const construct> block() const;

protected:
    mutable std::optional<std::shared_ptr<const if_t>> if_child;
};


#endif
