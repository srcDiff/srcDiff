#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP

#include "srcMLUtility.hpp"

// converts source code to srcML
xmlBuffer * translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlNodePtr unit_old, xmlNodePtr unit_new);

#endif
