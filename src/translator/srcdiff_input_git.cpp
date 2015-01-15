#include <srcdiff_input_git.hpp>

#include <srcml_converter.hpp>

#include <functional>
#include <cstdio>

class no_file_exception {};

srcdiff_input_git::srcdiff_input_git(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options, const srcdiff_input_source_git & git_input)
  : srcdiff_input(archive, input_path, options), git_input(git_input) {}

srcdiff_input_git::~srcdiff_input_git() {}

srcml_nodes srcdiff_input_git::input_nodes(int stream_source) const {

 if(!input_path || input_path->empty()) throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  void * context = git_input.open(input_path->c_str());

  std::string path = input_path->substr(0, input_path->find('@'));

  converter.convert(srcml_archive_check_extension(archive, path.c_str()), (void *)context, srcdiff_input_source_git::read, srcdiff_input_source_git::close, options);

  return converter.create_nodes();

}
