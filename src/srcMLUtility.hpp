#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP

#include "srcDiffUtility.hpp"
#include <Options.hpp>
#include <vector>

#include "xmlrw.hpp"

#include "pthread.h"

struct create_nodes_args {

  // args
  const char* src_encoding;
  const char* xml_encoding;
  xmlBuffer* output_buffer;
  OPTION_TYPE& options;
  const char* directory;
  const char* filename;
  const char* version;
  const char** uri;
  int tabsize;

  // pthreads
  pthread_mutex_t * mutex;

  // returns
  std::vector<xNode *> & nodes;
  xNodePtr * unit_start;
  int & no_error;
  int context;

};

// converts source code to srcML
void translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir, xmlBuffer* output_buffer);

void translate_to_srcML(const char* src_encoding, const char* xml_encoding, xmlBuffer* output_buffer, OPTION_TYPE& options,
                        const char* directory, const char* filename, const char* version, const char* uri[], int tabsize);

void * create_nodes_from_srcML_thread(void * arguments);

void create_nodes_from_srcML(const char* src_encoding, const char* xml_encoding, xmlBuffer* output_buffer, OPTION_TYPE& options,
                             const char* directory, const char* filename, const char* version, const char* uri[], int tabsize,
                             pthread_mutex_t * mutex,
                             std::vector<xNode *> & nodes, xNodePtr * unit_start, int & no_error, int context);

// create srcdiff unit
xNodePtr create_srcdiff_unit(xNodePtr unit_old, xNodePtr unit_new);

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);
void merge_filename(xNodePtr unit_old, xNodePtr unit_new);

bool is_atomic_srcml(std::vector<xNodePtr> * nodes, unsigned start);
void collect_nodes(std::vector<xNode *> * nodes, xmlTextReaderPtr reader, OPTION_TYPE & options, int context, pthread_mutex_t * mutex);

#endif
