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

#include <optional>

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

class srcdiff_input_source {

protected:

  const srcdiff_options & options;
  srcdiff_translator * translator;

  int directory_length_original;
  int directory_length_modified;

  std::optional<std::string> unit_version;

private:

  static bool show_input;

  static size_t input_count;
  static size_t input_skipped;
  static size_t input_total;

public:

  srcdiff_input_source(const srcdiff_options & options);
  virtual ~srcdiff_input_source() {}

  virtual void consume() = 0;
  virtual const char * get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified) = 0;

  virtual void file(const std::optional<std::string> & path_original,
                    const std::optional<std::string> & path_modified);
  virtual void directory(const std::optional<std::string> & directory_original,
                         const std::optional<std::string> & directory_modified);
  virtual void files_from();

  virtual void process_file(const std::optional<std::string> & path_original,
                            const std::optional<std::string> & path_modified) = 0;
  virtual void process_directory(const std::optional<std::string> & directory_original,
                                 const std::optional<std::string> & directory_modified) = 0;
  virtual void process_files_from() = 0;

};

#endif
