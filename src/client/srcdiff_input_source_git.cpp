#include <srcdiff_input_source_git.hpp>

#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
  
srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options)
  : srcdiff_input_source(options), path(boost::filesystem::temp_directory_path()), repo(nullptr), oid_original({ 0 }), oid_modified({ 0 }), commit_original(0), commit_modified(0), tree_original(0), tree_modified(0) {

  std::string command("git clone https://github.com/srcML/srcDiff.git ");
  command += path.native();

  FILE * process = popen(command.c_str(), "r");
  pclose(process);

  int error = 0;

  error = git_repository_open(&repo, "temp_repo");
  if(error) throw std::string("Error Opening up temporary repository.");

  error = git_oid_fromstr(&oid_original, "40b85bebf15521f68be75574773a330b60f42745");
  if(error) throw std::string("Error getting base/original revision: ");

  error = git_oid_fromstr(&oid_modified, "40b85bebf15521f68be75574773a330b60f42745");
  if(error) throw std::string("Error getting base/original revision: ");

  git_commit_lookup(&commit_original, repo, &oid_original);
  if(error) throw std::string("Error looking up base/original commit.");

  git_commit_lookup(&commit_modified, repo, &oid_modified);
  if(error) throw std::string("Error looking up base/original commit.");

  error = git_commit_tree(&tree_original, commit_original);
  if(error) throw std::string("Error accessing git commit tree.");

   error = git_commit_tree(&tree_modified, commit_modified);
  if(error) throw std::string("Error accessing git commit tree.");

}

srcdiff_input_source_git::~srcdiff_input_source_git() {

  git_tree_free(tree_original);
  git_tree_free(tree_modified);

  git_commit_free(commit_original);
  git_commit_free(commit_modified);

  git_repository_free(repo);

  std::string command("rm -rf ");
  command += path.native();
  
  FILE * process = popen(command.c_str(), "r");
  pclose(process);

}

void srcdiff_input_source_git::consume() {}

void srcdiff_input_source_git::file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new) {}

void srcdiff_input_source_git::directory(const boost::optional<std::string> & directory_old, int directory_length_old, const boost::optional<std::string> & directory_new, int directory_length_new) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif


#undef PATH_SEPARATOR

}

void srcdiff_input_source_git::files_from() {}

srcdiff_input_source_git::git_context * srcdiff_input_source_git::open(const char * uri) const {

  git_context * context = new git_context;


  return context;

}

int srcdiff_input_source_git::read(void * context, char * buffer, int len) {


  return 0;
}

int srcdiff_input_source_git::close(void * context) {

  git_context * ctx = (git_context *)context;

  delete ctx;

  return 1;
}
