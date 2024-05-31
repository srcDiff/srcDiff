/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_ACCESS_REGION_HPP
#define INCLUDED_ACCESS_REGION_HPP

#include <always_matched_construct.hpp>

class access_region : public always_matched_construct {

public:

    access_region(const construct* parent, std::size_t& start)
        : always_matched_construct(parent, start) {}
    virtual bool is_tag_convertable(const construct & modified) const;
private:

};


#endif
