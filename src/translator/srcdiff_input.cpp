#include <srcdiff_input.hpp>

srcdiff_input::srcdiff_input(srcml_archive * archive) {}

srcdiff_input~srcdiff_input() {}

void operator()(const char * input_path, int stream_source, std::vector<xNodePtr> & nodes, int & is_input) {

  is_input = 0;
  try {

    nodes = input_one.input_nodes(path_one, SESDELETE);
    is_input = 1;

  } catch(no_file_exception) {}
  catch(...) {

    is_input = -2;

  }


}

std::vector<xNodePtr> srcdiff_input::input_nodes(const char * input_path, int stream_source) {

  srcml_translator translator(archive, stream_source);

  translator.translate(input_path, options);

  return translator.create_nodes();

}