// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file elseif.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_ELSEIF_HPP
#define INCLUDED_ELSEIF_HPP

#include <clause.hpp>
#include <if.hpp>

class elseif : public clause {

public:

    elseif(const construct* parent, std::size_t& start)
        : clause(parent, start), if_child() {
    }


    std::shared_ptr<const if_t> find_if() const;

    virtual std::shared_ptr<const construct> condition() const;
    virtual std::shared_ptr<const construct> block() const;

protected:
    mutable std::optional<std::shared_ptr<const if_t>> if_child;
};


#endif
