/*
  srcdiff_input_source_local.hpp

  Functions for reading input files from the local file system

  Michael J. Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_LOCAL_HPP

#include <srcdiff_input_source.hpp>

#include <fstream>

#include <sys/stat.h>
#include <filesystem>

class srcdiff_input_source_local : public srcdiff_input_source {

protected:

  std::filesystem::directory_entry output_file;

  static int is_dir(std::filesystem::directory_entry d);

public:

  srcdiff_input_source_local(const srcdiff_options & options);
  virtual ~srcdiff_input_source_local();

  virtual void consume();

  virtual const char * get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified);

  virtual void process_file(const std::optional<std::string> & path_original,
                            const std::optional<std::string> & path_modified);
  virtual void process_directory(const std::optional<std::string> & directory_original,
                                 const std::optional<std::string> & directory_modified);
  virtual void process_files_from();

  struct input_context {

    std::ifstream in;

  };

  input_context * open(const char * uri) const;
  static ssize_t read(void * context, void * buffer, size_t len);
  static int close(void * context);

};

#endif
