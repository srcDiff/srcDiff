/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CHARACTER_DIFF_HPP
#define INCLUDED_CHARACTER_DIFF_HPP

#include <srcdiff_shortest_edit_script.hpp>
#include <versioned_string.hpp>

class view_t;

class character_diff {

private:

    srcdiff_shortest_edit_script ses;
    const versioned_string & str;

public:

    character_diff(const versioned_string & original);

    void compute();
	void output(view_t & view, const std::string & type);

};


#endif
