#include <srcdiff_input.hpp>

#include <srcml_converter.hpp>

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

std::vector<xNodePtr> srcdiff_input::input_nodes(const char * input_path, int stream_source) {

  srcml_converter converter(archive, stream_source);

  converter.convert(input_path, options);

  return converter.create_nodes();

}