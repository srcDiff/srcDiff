// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcml_nodes.hpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCML_NODES_HPP
#define INCLUDED_SRCML_NODES_HPP

#include <node.hpp>

#include <vector>
#include <memory>

typedef std::vector<std::shared_ptr<srcML::node>> srcml_nodes;

void advance_to_child(const srcml_nodes & nodes,
                      size_t & pos,
                      srcML::node_type type,
                      const std::string & name);

#endif
