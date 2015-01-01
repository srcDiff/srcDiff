/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SRCDIFF_LOCAL_INPUT_HPP
#define INCLUDED_SRCDIFF_LOCAL_INPUT_HPP

#include <srcdiff_source_input.hpp>

#include <sys/stat.h>

class srcdiff_local_input : public srcdiff_source_input {

protected:

private:

  struct stat outstat;

public:

  srcdiff_local_input(srcdiff_options & options);
  virtual ~srcdiff_local_input();

  virtual void consume();

  virtual void file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new);
  virtual void directory(const boost::optional<std::string> & directory_old, int directory_length_old, const boost::optional<std::string> & directory_new, int directory_length_new);
  virtual void files_from();

};

#endif
