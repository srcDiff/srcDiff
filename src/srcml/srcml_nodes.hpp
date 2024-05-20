/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
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
