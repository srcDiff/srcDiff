// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file delta_utils.hpp
 *
 * @copyright Copyright (C) 2024-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_DELTA_UTILS_HPP
#define INCLUDED_DELTA_UTILS_HPP

#include <srcml.h>

#include <string>

std::string serialize_unit(srcml_unit* unit);

std::string extract_revision_source(const std::string& srcdiff, int revision);

std::string switch_differences(const std::string& srcdiff);

#endif
