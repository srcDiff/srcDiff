#ifdef SVN

#include <srcdiff_input_svn.hpp>

#include <srcml_converter.hpp>

#include <srcdiff_svn_input.hpp>

#include <functional>
#include <cstdio>

class no_file_exception {};

srcdiff_input_svn::srcdiff_input_svn(srcml_archive * archive, const char * input_path, OPTION_TYPE options, const srcdiff_svn_input & svn_input)
  : srcdiff_input(archive, input_path, options), svn_input(svn_input) {}

srcdiff_input_svn::~srcdiff_input_svn() {}

std::vector<xNodePtr> srcdiff_input_svn::input_nodes(const char * input_path, int stream_source) {

  if(input_path == 0 || input_path[0] == 0 || input_path[0] == '@') throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  void * context = svn_input.open(input_path);

  const char * end = index(input_path, '@');
  const char * filename = strndup(input_path, end - input_path);

  converter.convert(filename, (void *)context, srcdiff_svn_input::read, srcdiff_svn_input::close, options);
  free((void *)filename);

  return converter.create_nodes();

}

#endif
