#include <srcdiff_input_source_git.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h>
  
srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options)
  : srcdiff_input_source(options), path(boost::filesystem::temp_directory_path().native() + boost::filesystem::unique_path().native()), repo(nullptr), oid_original({ 0 }), oid_modified({ 0 }), commit_original(0), commit_modified(0), tree_original(0), tree_modified(0) {

  std::string command("git clone https://github.com/mjdecker/srcDiff.git ");
  command += path.native();

  FILE * process = popen(command.c_str(), "r");
  pclose(process);

  int error = 0;

  error = git_repository_open(&repo, path.c_str());
  if(error) throw std::string("Error Opening up temporary repository.");

  error = git_oid_fromstr(&oid_original, "a0854ae1fcca1526775be59a61e0b87494e891e4");
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

void srcdiff_input_source_git::consume() {

  directory(std::string(), 0, (const void *)tree_original, std::string(), 0, (const void *)tree_modified);

}

void srcdiff_input_source_git::file(const boost::optional<std::string> & path_one, const boost::optional<std::string> & path_two, int directory_length_old, int directory_length_new) {

  std::string path_original = path_one ? *path_one : "";
  std::string path_modified = path_two ? *path_two : "";

  std::string unit_filename = !path_original.empty() ? path_original.substr(directory_length_old) : std::string();
  std::string filename_two =  !path_modified.empty() ? path_modified.substr(directory_length_new) : std::string();
  if(path_modified.empty() || unit_filename != filename_two) {

    unit_filename += "|";
    unit_filename += filename_two;

  }

  if(srcml_archive_check_extension(options.archive, path_original.c_str()) == SRCML_LANGUAGE_NONE
    && srcml_archive_check_extension(options.archive, path_modified.c_str()) == SRCML_LANGUAGE_NONE)
    return;

}

bool operator<(const std::pair<std::string, size_t> & pair_one, const std::pair<std::string, size_t> & pair_two) {

  return pair_one.first < pair_two.first;

}

void srcdiff_input_source_git::directory(const boost::optional<std::string> & directory_old, int directory_length_old, const void * context_old,
                                         const boost::optional<std::string> & directory_new, int directory_length_new, const void * context_new) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

  git_tree * current_tree_original = (git_tree *)context_old;
  git_tree * current_tree_modified = (git_tree *)context_new;

  size_t count_original = git_tree_entrycount(current_tree_original);
  std::vector<std::pair<std::string, size_t>> names_original;
  names_original.reserve(count_original);
  for(size_t i = 0; i < count_original; ++i) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, i);
    if (entry_original) names_original.push_back(std::make_pair(git_tree_entry_name(entry_original), i));

  }
  sort(names_original.begin(), names_original.end(), operator<);

  size_t count_modified = git_tree_entrycount(current_tree_modified);
  std::vector<std::pair<std::string, size_t>> names_modified;
  names_modified.reserve(count_modified);
  for(size_t i = 0; i < count_modified; ++i) {

    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, i);
    if (entry_modified) names_modified.push_back(std::make_pair(git_tree_entry_name(entry_modified), i));

  }
  sort(names_modified.begin(), names_modified.end(), operator<);

  // process files first

  std::vector<std::pair<std::string, size_t>>::size_type pos_original = 0;
  std::vector<std::pair<std::string, size_t>>::size_type pos_modified = 0;
  while(pos_original < count_original && pos_modified < count_modified) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, names_original.at(pos_original).second);
    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, names_modified.at(pos_modified).second);

    if(!entry_original || git_tree_entry_type(entry_original) == GIT_OBJ_TREE) { ++pos_original; continue; }
    if(!entry_modified || git_tree_entry_type(entry_modified) == GIT_OBJ_TREE) { ++pos_modified; continue; }

    int comparison = names_original.at(pos_original).first.compare(names_modified.at(pos_modified).first);

    boost::optional<std::string> path_original;
    boost::optional<std::string> path_modified;
    if(comparison <= 0) path_original = names_original.at(pos_original).first, ++pos_original;
    if(comparison >= 0) path_modified = names_modified.at(pos_modified).first, ++pos_modified;

    file(path_original, path_modified, directory_length_old, directory_length_new);

  }

  while(pos_original < count_original) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, names_original.at(pos_original).second);
    if(!entry_original || git_tree_entry_type(entry_original) == GIT_OBJ_TREE) { ++pos_original; continue; }

    boost::optional<std::string> path_original = names_original.at(pos_original).first;
    file(path_original, boost::optional<std::string>(), directory_length_old, directory_length_new);

    ++pos_original;

  }

  while(pos_modified < count_modified) {

    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, names_modified.at(pos_modified).second);
    if(!entry_modified || git_tree_entry_type(entry_modified) == GIT_OBJ_TREE) { ++pos_modified; continue; }

    boost::optional<std::string> path_modified = names_modified.at(pos_modified).first;
    file(boost::optional<std::string>(), path_modified, directory_length_old, directory_length_new);

    ++pos_modified;

  }

  // process directories

  pos_original = 0;
  pos_modified = 0;
  while(pos_original < count_original && pos_modified < count_modified) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, names_original.at(pos_original).second);
    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, names_modified.at(pos_modified).second);

    if(!entry_original || git_tree_entry_type(entry_original) != GIT_OBJ_TREE) { ++pos_original; continue; }
    if(!entry_modified || git_tree_entry_type(entry_modified) != GIT_OBJ_TREE) { ++pos_modified; continue; }

    int comparison = names_original.at(pos_original).first.compare(names_modified.at(pos_modified).first);

    boost::optional<std::string> path_original;
    boost::optional<std::string> path_modified;

    git_tree * subtree_original = nullptr;
    git_tree * subtree_modified = nullptr;

    if(comparison <= 0) {

      path_original = names_original.at(pos_original).first;
      int error = git_tree_lookup(&subtree_original, repo, git_tree_entry_id(entry_original));
      if(error) throw std::string("Error accessing git commit tree.");   

      ++pos_original;

    }

    if(comparison >= 0) {

      path_modified = names_modified.at(pos_modified).first;
      int error = git_tree_lookup(&subtree_modified, repo, git_tree_entry_id(entry_modified));
      if(error) throw std::string("Error accessing git commit tree.");

      ++pos_modified; 

    }

    directory(path_original, directory_length_old, subtree_original, path_modified, directory_length_new, subtree_modified);

    if(subtree_original) git_tree_free(subtree_original);
    if(subtree_modified) git_tree_free(subtree_modified);

  }

  while(pos_original < count_original) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, names_original.at(pos_original).second);
    if(!entry_original || git_tree_entry_type(entry_original) != GIT_OBJ_TREE) { ++pos_original; continue; }

    boost::optional<std::string> path_original = names_original.at(pos_original).first;
    git_tree * subtree_original = nullptr;
    int error = git_tree_lookup(&subtree_original, repo, git_tree_entry_id(entry_original));
    if(error) throw std::string("Error accessing git commit tree.");       

    directory(path_original, directory_length_old, subtree_original, boost::optional<std::string>(), directory_length_new, nullptr);

    if(subtree_original) git_tree_free(subtree_original);

    ++pos_original;

  }

  while(pos_modified < count_modified) {

    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, names_modified.at(pos_modified).second);
    if(!entry_modified || git_tree_entry_type(entry_modified) != GIT_OBJ_TREE) { ++pos_modified; continue; }

    boost::optional<std::string> path_modified = names_modified.at(pos_modified).first;
    git_tree * subtree_modified = nullptr;
    int error = git_tree_lookup(&subtree_modified, repo, git_tree_entry_id(entry_modified));
    if(error) throw std::string("Error accessing git commit tree.");

    directory(boost::optional<std::string>(), directory_length_old, nullptr, path_modified, directory_length_new, subtree_modified);

    if(subtree_modified) git_tree_free(subtree_modified);

    ++pos_modified;

  }

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
