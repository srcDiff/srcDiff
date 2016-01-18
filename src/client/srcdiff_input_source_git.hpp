
#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP

#include <srcdiff_input_source.hpp>

#include <git2.h>

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

  virtual void process_file(const boost::optional<std::string> & path_original, const void * context_original,
                            const boost::optional<std::string> & path_modified, const void * context_modified);
  virtual void process_directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                 const boost::optional<std::string> & directory_modified, const void * context_modified);
  virtual void process_files_from();

  struct input_context  {

    std::ifstream in;

  };

  input_context * open(const char * uri) const;
  static int read(void * context, char * buffer, size_t len);
  static int close(void * context);

};

#endif

#endif
