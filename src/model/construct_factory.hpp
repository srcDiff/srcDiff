/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONSTRUCT_FACTORY_HPP
#define INCLUDED_CONSTRUCT_FACTORY_HPP

class construct;
#include <srcdiff_output.hpp>

#include <string>
#include <memory>

std::shared_ptr<construct> create_construct(const srcml_nodes & node_list, std::size_t & start, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>());

#endif
