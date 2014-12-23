#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP


#include <srcml.h>

#include <vector>
#include <thread>

#include <Options.hpp>
#include <xmlrw.hpp>



class no_file_exception {};

class srcml_converter {

protected:

  srcml_archive * archive;
  OPTION_TYPE options;

  int stream_source;

  char * output_buffer;
  int output_size;

private:

std::vector<xNodePtr> collect_nodes(xmlTextReaderPtr reader);

public:

  srcml_converter(srcml_archive * archive, int stream_source);
  ~srcml_converter();

  void convert(const char * path, const char * filename, OPTION_TYPE options); 
  std::vector<xNodePtr> create_nodes();

};

#endif
