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

  int directory_length_original;
  int directory_length_modified;

private:

  static bool show_input;

  static size_t input_count;
  static size_t input_skipped;
  static size_t input_total;

public:

  srcdiff_input_source(const srcdiff_options & options) : options(options), translator(0), directory_length_original(0), directory_length_modified(0) {

  show_input = is_option(options.flags, OPTION_VERBOSE) && !is_option(options.flags, OPTION_QUIET);

  }


  virtual ~srcdiff_input_source() {}

  virtual void consume() = 0;
  virtual const char * get_language(const boost::optional<std::string> & path_original, const boost::optional<std::string> & path_modified) = 0;

  virtual void file(const boost::optional<std::string> & path_original, const void * context_original,
                    const boost::optional<std::string> & path_modified, const void * context_modified);
  virtual void directory(const boost::optional<std::string> & directory_original, const void * context_original,
                         const boost::optional<std::string> & directory_modified, const void * context_modified);
  virtual void files_from();

  virtual void process_file(const boost::optional<std::string> & path_original, const void * context_original,
                            const boost::optional<std::string> & path_modified, const void * context_modified) = 0;
  virtual void process_directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                 const boost::optional<std::string> & directory_modified, const void * context_modified) = 0;
  virtual void process_files_from() = 0;

};

#endif
