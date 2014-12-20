#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP


#include <srcml.h>

#include <vector>
#include <Options.hpp>

#include <xmlrw.hpp>

#include <pthread.h>


class no_file_exception {};

class srcml_translator {

protected:

  srcml_archive * archive;
  OPTION_TYPE options;

  int stream_source;

  char * output_buffer;
  int output_size;

private:

std::vector<xNode *> collect_nodes(xmlTextReaderPtr reader, pthread_mutex_t * mutex);

public:

  srcml_translator(srcml_archive * archive, int stream_source);
  ~srcml_translator();

  void translate(const char* path, const char * language, const char * filename, const char * directory, const char * version, OPTION_TYPE options); 
  std::vector<xNode *> create_nodes_from_srcml(pthread_mutex_t * mutex);

};

#endif
