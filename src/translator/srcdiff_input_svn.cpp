#ifdef SVN

#include <srcdiff_input_svn.hpp>

#include <srcml_converter.hpp>

#include <functional>
#include <cstdio>

class no_file_exception {};

srcdiff_input_svn::srcdiff_input_svn(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options, const srcdiff_input_source_svn & svn_input)
  : srcdiff_input(archive, input_path, options), svn_input(svn_input) {}

srcdiff_input_svn::~srcdiff_input_svn() {}

std::vector<xNodePtr> srcdiff_input_svn::input_nodes(int stream_source) const {

 if(!input_path || input_path->empty()) throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  void * context = svn_input.open(input_path->c_str());

  std::string path = input_path->substr(0, input_path->find('@'));

  converter.convert(srcml_archive_check_extension(archive, path.c_str()), (void *)context, srcdiff_input_source_svn::read, srcdiff_input_source_svn::close, options);

  return converter.create_nodes();

}

#endif
