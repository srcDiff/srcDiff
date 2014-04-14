#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP

#include <srcDiffUtility.hpp>
#include <Options.hpp>
#include <vector>

#include <xmlrw.hpp>

#include <pthread.h>

#include <srcml.h>

struct create_nodes_args {

  // args
  const char* path;
  srcml_archive * main_archive;
  srcml_unit * unit;

  // pthreads
  pthread_mutex_t * mutex;

  // returns
  std::vector<xNode *> & nodes;
  int & no_error;
  int context;

};

// converts source code to srcML
void translate_to_srcML(const char * path, srcml_archive * main_archive, srcml_unit * unit,
                   			char ** output_buffer, int * output_size); 

void * create_nodes_from_srcML_thread(void * arguments);

void create_nodes_from_srcML(const char * path, srcml_archive * main_archive, srcml_unit * unit,
                             pthread_mutex_t * mutex,
                             std::vector<xNode *> & node, int & no_error, int context);

// create srcdiff unit
xNodePtr create_srcdiff_unit(xNodePtr unit_old, xNodePtr unit_new);

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);
void merge_filename(xNodePtr unit_old, xNodePtr unit_new);

bool is_atomic_srcml(std::vector<xNodePtr> * nodes, unsigned start);
void collect_nodes(std::vector<xNode *> * nodes, xmlTextReaderPtr reader, OPTION_TYPE options, int context, pthread_mutex_t * mutex);

#endif
