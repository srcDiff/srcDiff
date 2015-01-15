/*
  svn_io.hpp

  Functions for reading a file form svn

  Michael J. Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_HPP

#include <srcdiff_options.hpp>
#include <srcdiff_translator.hpp>

#include <string>

#include <boost/optional.hpp>

class srcdiff_input_source {

protected:

  const srcdiff_options & options;
  srcdiff_translator * translator;

private:

public:

  srcdiff_input_source(const srcdiff_options & options) : options(options), translator(0) {}
  virtual ~srcdiff_input_source() {}

  virtual void consume() = 0;

  virtual void file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new) = 0;
  virtual void directory(const boost::optional<std::string> & directory_old, int directory_length_old, const void * context_old,
                         const boost::optional<std::string> & directory_new, int directory_length_new, const void * context_new) = 0;
  virtual void files_from() = 0;

};

#endif
