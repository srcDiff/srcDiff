/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP

#include <srcdiff_input_source.hpp>

#include <fstream>

#ifndef _MSC_BUILD
#include <dirent.h>
#endif
#include <sys/stat.h>

class srcdiff_input_source_local : public srcdiff_input_source {

protected:

  struct stat outstat;

#ifndef _MSC_BUILD
  static int dir_filter(const struct dirent* d);
  static int dir_filter(struct dirent* d);
  static int is_dir(struct dirent * file, const char * filename);
#endif
  static int is_output_file(const char * filename, const struct stat & outstat);

public:

  srcdiff_input_source_local(const srcdiff_options & options);
  virtual ~srcdiff_input_source_local();

  virtual void consume();

  virtual const char * get_language(const boost::optional<std::string> & path_original, const boost::optional<std::string> & path_modified);

  virtual void process_file(const boost::optional<std::string> & path_original,
                            const boost::optional<std::string> & path_modified);
  virtual void process_directory(const boost::optional<std::string> & directory_original,
                                 const boost::optional<std::string> & directory_modified);
  virtual void process_files_from();

  struct input_context {

    std::ifstream in;

  };

  input_context * open(const char * uri) const;
  static ssize_t read(void * context, void * buffer, size_t len);
  static int close(void * context);

};

#endif
