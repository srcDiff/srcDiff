
#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP

#include <srcdiff_input_source_local.hpp>

#include <boost/filesystem.hpp>

class srcdiff_input_source_git : public srcdiff_input_source_local {

protected:

private:

  boost::filesystem::path original_clone_path;
  boost::filesystem::path modified_clone_path;
  bool clean_path;

public:

  srcdiff_input_source_git(const srcdiff_options & options);

  virtual ~srcdiff_input_source_git();

  virtual void consume();

  virtual const char * get_language(const boost::optional<std::string> & path_original, const boost::optional<std::string> & path_modified);

  virtual void process_file(const boost::optional<std::string> & path_original,
                            const boost::optional<std::string> & path_modified);
  virtual void process_directory(const boost::optional<std::string> & directory_original,
                                 const boost::optional<std::string> & directory_modified);
  virtual void process_files_from();

};

#endif

#endif
