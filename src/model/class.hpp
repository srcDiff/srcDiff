/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CLASS_HPP
#define INCLUDED_CLASS_HPP

#include <named_construct.hpp>

class class_t : public named_construct {

public:

    class_t(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), named_construct(node_list, start, out) {}
    virtual bool is_tag_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;
private:

};


#endif
