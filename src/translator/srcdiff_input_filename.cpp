#include <srcdiff_input_filename.hpp>

#include <srcml_converter.hpp>

#include <functional>
#include <cstdio>

class no_file_exception {};

srcdiff_input_filename::srcdiff_input_filename(srcml_archive * archive, const boost::optional<std::string> &, const OPTION_TYPE & options) : srcdiff_input(archive, input_path, options) {}

srcdiff_input_filename::~srcdiff_input_filename() {}

static int file_read(void * context, char * buffer, int len) {

    return fread((void *)buffer, 1, len, (FILE *)context);

}

static int file_close(void * context) {

  return fclose((FILE *) context);

 }

std::vector<xNodePtr> srcdiff_input_filename::input_nodes(const boost::optional<std::string> &, int stream_source) {

  if(!input_path || input_path->empty()) throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  FILE * context = fopen(input_path->c_str(), "r");

  converter.convert(*input_path, (void *)context, file_read, file_close, options);

  return converter.create_nodes();

}
