// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file unit.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_UNIT_HPP
#define INCLUDED_UNIT_HPP


#include <construct.hpp>

class unit : public construct {

public:

    unit(const srcml_nodes & node_list, std::shared_ptr<srcdiff_output> out) : construct(node_list, out) {
        terms.push_back(-1);
        terms.push_back(node_list.size());
    }

};


#endif
