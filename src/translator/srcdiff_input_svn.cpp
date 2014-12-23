#ifdef SVN

#include <srcdiff_input_svn.hpp>

#include <srcml_converter.hpp>

#include <svn_io.hpp>

#include <functional>
#include <cstdio>

class no_file_exception {};

srcdiff_input_svn::srcdiff_input_svn(srcml_archive * archive, const char * input_path, OPTION_TYPE options) : srcdiff_input(archive, input_path, options) {}

srcdiff_input_svn::~srcdiff_input_svn() {}

static int file_read(void * context, char * buffer, int len) {

    return fread((void *)buffer, 1, len, (FILE *)context);

}

static int file_close(void * context) {

  return fclose((FILE *) context);

 }

std::vector<xNodePtr> srcdiff_input_svn::input_nodes(const char * input_path, int stream_source) {

  if(input_path == 0 || input_path[0] == 0 || input_path[0] == '@') throw no_file_exception();

  srcml_converter converter(archive, stream_source);

  void * context = svnReadOpen(path);

  const char * end = index(path, '@');
  const char * filename = strndup(path, end - path);
  srcml_unit_set_language(unit, srcml_archive_check_extension(unit_archive, filename));

  converter.convert(filename, (void *)context, svnRead, svnReadClose, options);
  free((void *)filename);

  return converter.create_nodes();

}

#endif
