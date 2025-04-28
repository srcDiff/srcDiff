// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file construct_factory.hpp
 *
 * @copyright Copyright (C) 2023-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONSTRUCT_FACTORY_HPP
#define INCLUDED_CONSTRUCT_FACTORY_HPP

class construct;
#include <output_stream.hpp>

#include <string>
#include <memory>

std::shared_ptr<construct> create_construct(const construct* parent, std::size_t & start);

#endif
