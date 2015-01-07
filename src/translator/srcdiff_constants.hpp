#ifndef INCLUDED_SRCDIFF_CONSTANTS_HPP
#define INCLUDED_SRCDIFF_CONSTANTS_HPP

#include <string>

const std::string DIFF_PREFIX = "diff:";
const std::string DIFF_OLD = "delete";
const std::string DIFF_NEW = "insert";
const std::string DIFF_SESCOMMON = "common";
const std::string DIFF_TYPE = "type";

const std::string SRCDIFF_DEFAULT_NAMESPACE_HREF = "http://www.sdml.info/srcDiff";
const std::string SRCDIFF_DEFAULT_NAMESPACE_PREFIX =  "diff";

const int MAX_INT = (unsigned)-1 >> 1;

#endif
