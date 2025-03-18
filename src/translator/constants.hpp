// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file constants.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CONSTANTS_HPP
#define INCLUDED_CONSTANTS_HPP

#include <string>

namespace srcdiff {

const std::string DIFF_PREFIX = "diff:";
const std::string DIFF_ORIGINAL = "delete";
const std::string DIFF_MODIFIED = "insert";
const std::string DIFF_SES_COMMON = "common";
const std::string DIFF_TYPE = "type";

const std::string DIFF_WHITESPACE = "ws";

const std::string SRCDIFF_DEFAULT_NAMESPACE_HREF = "http://www.srcML.org/srcDiff";
const std::string SRCDIFF_DEFAULT_NAMESPACE_PREFIX =  "diff";

const std::string SRCML_SRC_NAMESPACE_HREF = "http://www.srcML.org/srcML/src";
const std::string SRCML_CPP_NAMESPACE_PREFIX = "cpp";
const std::string SRCML_CPP_NAMESPACE_HREF = "http://www.srcML.org/srcML/cpp";

const int MAX_INT = (unsigned)-1 >> 1;

}

#endif
