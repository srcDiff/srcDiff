// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file block.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_BLOCK_HPP
#define INCLUDED_BLOCK_HPP

#include <construct.hpp>

class block : public construct {

public:

    block(const construct* parent, std::size_t& start)
        : construct(parent, start) {}

    std::shared_ptr<const construct> block_content() const;

    bool is_syntax_similar_impl(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;
private:
    mutable std::optional<std::shared_ptr<const construct>> block_content_child;
};

#endif
