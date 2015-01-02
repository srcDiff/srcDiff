#include <srcdiff_input.hpp>

class no_file_exception {};

srcdiff_input::srcdiff_input(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options)
	: archive(archive), input_path(input_path), options(options) {}

srcdiff_input::~srcdiff_input() {}

void srcdiff_input::operator()(int stream_source, std::vector<xNodePtr> & nodes, int & is_input) {

  is_input = 0;
  try {

    nodes = input_nodes(input_path, stream_source);
    is_input = 1;

  } catch(no_file_exception) {}
  catch(...) {

    is_input = -2;

  }

}
