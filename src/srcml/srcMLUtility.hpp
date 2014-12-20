#ifndef INCLUDED_SRCMLUTILITY_HPP
#define INCLUDED_SRCMLUTILITY_HPP


#include <srcml.h>

#include <vector>
#include <thread>

#include <Options.hpp>
#include <xmlrw.hpp>



class no_file_exception {};

class srcml_translator {

protected:

  srcml_archive * archive;
  OPTION_TYPE options;

  int stream_source;

  char * output_buffer;
  int output_size;

  std::mutex mutex;

private:

std::vector<xNodePtr> collect_nodes(xmlTextReaderPtr reader);

public:

  srcml_translator(srcml_archive * archive, int stream_source);
  ~srcml_translator();

  void translate(const char* path, OPTION_TYPE options); 
  std::vector<xNodePtr> create_nodes();

};

#endif
