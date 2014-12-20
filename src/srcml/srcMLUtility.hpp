#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP

#include <srcDiffUtility.hpp>
#include <Options.hpp>
#include <vector>

#include <xmlrw.hpp>

#include <pthread.h>

#include <srcml.h>

class no_file_exception {};

class srcml_translator {

protected:

  char ** output_buffer;
  int * output_size;
  OPTION_TYPE options;

  srcml_archive * archive;

  int stream_source;

private:

static bool is_atomic_srcml(std::vector<xNodePtr> * nodes, unsigned start);
static void collect_nodes(std::vector<xNode *> * nodes, xmlTextReaderPtr reader, OPTION_TYPE options, int context, pthread_mutex_t * mutex);
static xNodePtr create_srcdiff_unit(xNodePtr unit_old, xNodePtr unit_new);

public:

  srcml_translator(srcml_archive * archive);

  void translate(const char* path, const char * language, const char * filename, const char * directory, const char * version); 
  void create_nodes_from_srcml(pthread_mutex_t * mutex, std::vector<xNode *> & node);

};

#endif
