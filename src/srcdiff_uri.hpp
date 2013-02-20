#ifndef INCLUDED_SRCDIFF_URI_HPP
#define INCLUDED_SRCDIFF_URI_HPP

#include "srcmlapps.hpp"
#include "srcmlns.hpp"
#include "Options.hpp"

struct uridata {
  char const * const uri;
  char const * const prefix;
  int option;
  char const * const description;
};

const uridata uris[] = {

  { SRCML_SRC_NS_URI,          SRCML_SRC_NS_PREFIX_DEFAULT, 0,               "primary srcML namespace" },
  { SRCML_CPP_NS_URI,          SRCML_CPP_NS_PREFIX_DEFAULT, OPTION_CPP,      "namespace for cpreprocessing elements" },
  { SRCML_ERR_NS_URI,          SRCML_ERR_NS_PREFIX_DEFAULT, OPTION_DEBUG,    "namespace for srcML debugging elements" },
  { SRCML_EXT_LITERAL_NS_URI,  SRCML_EXT_LITERAL_NS_PREFIX_DEFAULT, OPTION_LITERAL,  "namespace for optional literal elements" },  { SRCML_EXT_OPERATOR_NS_URI, SRCML_EXT_OPERATOR_NS_PREFIX_DEFAULT, OPTION_OPERATOR, "namespace for optional operator element"},
  { SRCML_EXT_MODIFIER_NS_URI, SRCML_EXT_MODIFIER_NS_PREFIX_DEFAULT, OPTION_MODIFIER, "namespace for optional modifier element"},
  { SRCML_EXT_POSITION_NS_URI, SRCML_EXT_POSITION_NS_PREFIX_DEFAULT, OPTION_POSITION, "namespace for optional position element and attributes" },
  { SRCML_DIFF_NS_URI,         SRCML_DIFF_NS_PREFIX_DEFAULT, 0,               "primary srcDiff namespace" },
};

const int num_prefixes = sizeof(uris) / sizeof(uris[0]);

#endif
