#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP

#include "srcDiffUtility.hpp"

// converts source code to srcML
xmlBuffer * translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlNodePtr unit_old, xmlNodePtr unit_new);

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);
void merge_filename(xmlNodePtr unit_old, xmlNodePtr unit_new);

bool is_atomic_srcml(std::vector<xmlNodePtr> * nodes, unsigned start);
  void collect_nodes(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader);

#endif
