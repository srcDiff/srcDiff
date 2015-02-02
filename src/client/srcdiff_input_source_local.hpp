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

#include <sys/stat.h>

class srcdiff_input_source_local : public srcdiff_input_source {

protected:

private:

  struct stat outstat;

public:

  srcdiff_input_source_local(const srcdiff_options & options);
  virtual ~srcdiff_input_source_local();

  virtual void consume();

  virtual const char * get_language(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two);

  virtual void process_file(const boost::optional<std::string> & path_one, const void * context_original,
                            const boost::optional<std::string> & path_two, const void * context_modified);
  virtual void process_directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                 const boost::optional<std::string> & directory_modified, const void * context_modified);
  virtual void files_from();

  struct input_context {

    std::ifstream in;

  };

  input_context * open(const char * uri) const;
  static int read(void * context, char * buffer, int len);
  static int close(void * context);

};

#endif
