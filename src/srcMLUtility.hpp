#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP

#include "srcDiffUtility.hpp"
#include <Options.hpp>
#include <vector>

// converts source code to srcML
void translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir, xmlBuffer* output_buffer);

void translate_to_srcML(int language, const char* src_encoding, const char* xml_encoding, xmlBuffer* output_buffer, OPTION_TYPE& options,
                        const char* directory, const char* filename, const char* version, const char* uri[], int tabsize);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlNodePtr unit_old, xmlNodePtr unit_new);

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);
void merge_filename(xmlNodePtr unit_old, xmlNodePtr unit_new);

bool is_atomic_srcml(std::vector<xmlNodePtr> * nodes, unsigned start);
void collect_nodes(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader);

#endif
