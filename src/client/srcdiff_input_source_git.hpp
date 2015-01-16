
#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_SOURCE_GIT_HPP

#include <srcdiff_input_source.hpp>

#include <git2.h>

#include <boost/filesystem.hpp>

class srcdiff_input_source_git : public srcdiff_input_source {

protected:

private:

  boost::filesystem::path clone_path;
  bool clean_path;

  git_repository * repo;

  git_oid oid_original;
  git_oid oid_modified;

  git_commit * commit_original;
  git_commit * commit_modified;

  git_tree * tree_original;
  git_tree * tree_modified;

public:

  srcdiff_input_source_git(const srcdiff_options & options);
  srcdiff_input_source_git(const srcdiff_options & options, const boost::optional<std::string> & local_path);

  virtual ~srcdiff_input_source_git();

  virtual void consume();

  virtual void file(const boost::optional<std::string> & path_one, const void * context_original,
                    const boost::optional<std::string> & path_two, const void * context_modified);
  virtual void directory(const boost::optional<std::string> & directory_original, const void * context_original,
                         const boost::optional<std::string> & directory_modified, const void * context_modified);
  virtual void files_from();

  struct git_context {

    git_blob * blob;
    git_buf content;

    size_t pos;

  };

  git_context * open(const char * uri) const;
  static int read(void * context, char * buffer, int len);
  static int close(void * context);

};

#endif

#endif
