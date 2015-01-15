/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP

#include <srcdiff_input_source.hpp>

#include <sys/stat.h>

class srcdiff_input_source_local : public srcdiff_input_source {

protected:

private:

  struct stat outstat;

public:

  srcdiff_input_source_local(const srcdiff_options & options);
  virtual ~srcdiff_input_source_local();

  virtual void consume();

  virtual void file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new);
  virtual void directory(const boost::optional<std::string> & directory_old, int directory_length_old, const void * context_old,
                         const boost::optional<std::string> & directory_new, int directory_length_new, const void * context_new);
  virtual void files_from();

};

#endif
