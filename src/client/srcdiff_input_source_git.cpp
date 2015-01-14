
#include <srcdiff_input_source_git.hpp>

#include <srcdiff_input_git.hpp>

#include <iostream>
#include <string>
#include <vector>
  
#include <URIStream.hpp>

srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options) : srcdiff_input_source(options) {

  std::string command = "git clone ";
  command += argv[1];
  command += " temp_repo";

  FILE * process = popen(command.c_str(), "r");
  pclose(process);

  int error = 0;

  git_repository * repo;
  error = git_repository_open(&repo, "temp_repo");
  if(error) throw std::string("Error Opening up temporary repository.");

  git_oid oid = { 0 };
  error = git_oid_fromstr(&oid, "40b85bebf15521f68be75574773a330b60f42745");
  if(error) throw std::string("Error getting revision: ");

  git_commit * commit;
  git_commit_lookup(&commit, repo, &oid);
  if(error) throw std:string("Error looking up base/original commit.");

  git_tree * tree;
  error = git_commit_tree(&tree, commit);
  if(error) throw std::string("Error accessing git commit tree.");


}

srcdiff_input_source_git::~srcdiff_input_source_git() {}

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
