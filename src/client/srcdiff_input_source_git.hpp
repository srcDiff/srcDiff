
//#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP

#include <srcdiff_input_source.hpp>

#include <git2.h>

#include <boost/filesystem.hpp>

class srcdiff_input_source_git : public srcdiff_input_source {

protected:

private:

  boost::filesystem::path path;

  git_repository * repo;

  git_oid oid_original;
  git_oid oid_modified;

  git_commit * commit_original;
  git_commit * commit_modified;

  git_tree * tree_original;
  git_tree * tree_modified;

public:

  srcdiff_input_source_git(const srcdiff_options & options);
  virtual ~srcdiff_input_source_git();

  virtual void consume();

  virtual void file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new);
  virtual void directory(const boost::optional<std::string> & directory_old, int directory_length_old, const boost::optional<std::string> & directory_new, int directory_length_new);
  virtual void files_from();

  struct git_context {

  };

  git_context * open(const char * uri) const;
  static int read(void * context, char * buffer, int len);
  static int close(void * context);

};

#endif

//#endif
