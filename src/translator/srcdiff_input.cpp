#include <srcdiff_input.hpp>

srcdiff_input::srcdiff_input(srcml_archive * archive) {}

srcdiff_input~srcdiff_input() {}

std::vector<xNodePtr> srcdiff_input::input_nodes(const char * input_path, int stream_source) {

  srcml_translator translator(archive, stream_source);

  translator.translate(input_path, options);

  return translator.create_nodes();

}