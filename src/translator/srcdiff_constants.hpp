#ifndef INCLUDED_SRCDIFF_CONSTANTS_HPP
#define INCLUDED_SRCDIFF_CONSTANTS_HPP

#include <string>

const std::string DIFF_PREFIX = "diff:";
const std::string DIFF_OLD = "delete";
const std::string DIFF_NEW = "insert";
const std::string DIFF_SES_COMMON = "common";
const std::string DIFF_TYPE = "type";

const std::string SRCDIFF_DEFAULT_NAMESPACE_HREF = "http://www.srcML.org/srcDiff";
const std::string SRCDIFF_DEFAULT_NAMESPACE_PREFIX =  "diff";

const std::string SRCML_SRC_NAMESPACE_HREF = "http://www.srcML.org/srcML/src";
const std::string SRCML_CPP_NAMESPACE_HREF = "http://www.srcML.org/srcML/cpp";

const int MAX_INT = (unsigned)-1 >> 1;

#endif
