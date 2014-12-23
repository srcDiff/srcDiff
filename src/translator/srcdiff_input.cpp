#include <srcdiff_input.hpp>

#include <srcml_converter.hpp>

#include <functional>
#include <cstdio>

class no_file_exception {};

srcdiff_input::srcdiff_input(srcml_archive * archive, OPTION_TYPE options) : archive(archive), options(options) {}

srcdiff_input::~srcdiff_input() {}

void srcdiff_input::operator()(const char * input_path, int stream_source, std::vector<xNodePtr> & nodes, int & is_input) {

  is_input = 0;
  try {

    nodes = input_nodes(input_path, stream_source);
    is_input = 1;

  } catch(no_file_exception) {}
  catch(...) {

    is_input = -2;

  }


}

static int file_read(void * context, char * buffer, int len) {

    return fread((void *)buffer, 1, len, (FILE *)context);

}

static int file_close(void * context) {

  return fclose((FILE *) context);

}

std::vector<xNodePtr> srcdiff_input::input_nodes(const char * input_path, int stream_source) {

  if(input_path == 0 || input_path[0] == 0 || input_path[0] == '@') throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  FILE * context = fopen(input_path, "r");

  converter.convert(input_path, (void *)context, file_read, file_close, options);

  return converter.create_nodes();

}