// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file identifier_decl.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_IDENTIFIER_DECL_HPP
#define INCLUDED_IDENTIFIER_DECL_HPP

#include <named_construct.hpp>

class identifier_decl : public named_construct {

public:

    identifier_decl(const construct* parent, std::size_t& start)
        : construct(parent, start), named_construct(parent, start) {
    }
    virtual std::shared_ptr<const name_t> name() const;

private:
    
};


#endif
