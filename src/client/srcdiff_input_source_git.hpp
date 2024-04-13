
#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP

#include <srcdiff_input_source_local.hpp>

#include <filesystem>

class srcdiff_input_source_git : public srcdiff_input_source_local {

protected:

private:

  long arg_max;

  std::filesystem::path original_clone_path;
  std::filesystem::path modified_clone_path;
  bool clean_path;

public:

  srcdiff_input_source_git(const srcdiff_options & options);

  virtual ~srcdiff_input_source_git();

  virtual void consume();

  virtual void process_file(const std::optional<std::string> & path_original,
                            const std::optional<std::string> & path_modified);
  virtual void process_directory(const std::optional<std::string> & directory_original,
                                 const std::optional<std::string> & directory_modified);
  virtual void process_files_from();

};

#endif

#endif
