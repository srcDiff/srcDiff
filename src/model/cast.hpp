/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CAST_HPP
#define INCLUDED_CAST_HPP

#include <named_construct.hpp>

class cast : public construct {

public:

    cast(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out) {}
    virtual bool is_tag_convertable(const construct & modified) const;
private:

};


#endif
