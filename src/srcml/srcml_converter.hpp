#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP


#include <srcml.h>

#include <vector>
#include <thread>
#include <functional>

#include <srcdiff_options.hpp>
#include <xmlrw.hpp>

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

  void convert(const std::string & path, void * context, std::function<int(void *, char *, int)> read, std::function<int(void *)> close, const OPTION_TYPE & options); 
  std::vector<xNodePtr> create_nodes();

};

#endif
