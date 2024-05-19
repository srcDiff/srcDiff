/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONDITIONAL_HPP
#define INCLUDED_CONDITIONAL_HPP

#include <construct.hpp>

#include <unordered_set>

class conditional : public construct {

public:

    conditional(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), condition_child() {}

    virtual std::shared_ptr<const construct> condition() const;
    virtual bool is_matchable_impl(const construct & modified) const;
    virtual bool is_tag_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;
protected:
    mutable std::optional<std::shared_ptr<const construct>> condition_child;

    static const std::unordered_set<std::string>conditional_convertable;
};


#endif
