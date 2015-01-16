#include <srcdiff_input_source_git.hpp>

#include <srcdiff_input_git.hpp>

#include <mutex>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <cstdio>

#include <URIStream.hpp>

static std::mutex mutex;
  
srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options)
  : srcdiff_input_source(options), clone_path(boost::filesystem::temp_directory_path().native() + boost::filesystem::unique_path().native()), clean_path(true), repo(nullptr), oid_original({ 0 }), oid_modified({ 0 }), commit_original(0), commit_modified(0), tree_original(0), tree_modified(0) {

  std::string command("git clone " + *options.git_url + " ");
  command += clone_path.native();

  FILE * process = popen(command.c_str(), "r");
  pclose(process);

  int error = 0;

  error = git_repository_open(&repo, clone_path.c_str());
  if(error) throw std::string("Error Opening up temporary repository.");

  error = git_oid_fromstr(&oid_original, options.git_revision_one.c_str());
  if(error) throw std::string("Error getting base/original revision: ");

  error = git_oid_fromstr(&oid_modified, options.git_revision_two.c_str());
  if(error) throw std::string("Error getting base/original revision: ");

  git_commit_lookup(&commit_original, repo, &oid_original);
  if(error) throw std::string("Error looking up base/original commit.");

  git_commit_lookup(&commit_modified, repo, &oid_modified);
  if(error) throw std::string("Error looking up base/original commit.");

  error = git_commit_tree(&tree_original, commit_original);
  if(error) throw std::string("Error accessing git commit tree.");

   error = git_commit_tree(&tree_modified, commit_modified);
  if(error) throw std::string("Error accessing git commit tree.");

  translator = new srcdiff_translator(options.srcdiff_filename,
                                options.flags, options.methods,
                                options.archive,
                                options.number_context_lines);

}

srcdiff_input_source_git::srcdiff_input_source_git(const srcdiff_options & options, const boost::optional<std::string> & local_path)
  : srcdiff_input_source(options), clone_path(*local_path), clean_path(false), repo(nullptr), oid_original({ 0 }), oid_modified({ 0 }), commit_original(0), commit_modified(0), tree_original(0), tree_modified(0)  {

  int error = 0;

  error = git_repository_open(&repo, clone_path.c_str());
  if(error) throw std::string("Error Opening up temporary repository.");

/*
  error = git_oid_fromstr(&oid_original, options.git_revision_one.c_str());
  if(error) throw std::string("Error getting base/original revision: ");

  error = git_oid_fromstr(&oid_modified, options.git_revision_two.c_str());
  if(error) throw std::string("Error getting base/original revision: ");

  git_commit_lookup(&commit_original, repo, &oid_original);
  if(error) throw std::string("Error looking up base/original commit.");

  git_commit_lookup(&commit_modified, repo, &oid_modified);
  if(error) throw std::string("Error looking up base/original commit.");

  error = git_commit_tree(&tree_original, commit_original);
  if(error) throw std::string("Error accessing git commit tree.");

   error = git_commit_tree(&tree_modified, commit_modified);
  if(error) throw std::string("Error accessing git commit tree.");

  translator = new srcdiff_translator(options.srcdiff_filename,
                                options.flags, options.methods,
                                options.archive,
                                options.number_context_lines);
  */

}

srcdiff_input_source_git::~srcdiff_input_source_git() {

  if(translator) delete translator;

  if(tree_original) git_tree_free(tree_original);
  if(tree_modified) git_tree_free(tree_modified);

  if(commit_original) git_commit_free(commit_original);
  if(commit_modified) git_commit_free(commit_modified);

  if(repo) git_repository_free(repo);

  if(clean_path) {

    std::string command("rm -rf ");
    command += clone_path.native();

    FILE * process = popen(command.c_str(), "r");
    pclose(process);

  }

}

void srcdiff_input_source_git::consume() {

  directory(std::string(), (const void *)tree_original, std::string(), (const void *)tree_modified);

}

void srcdiff_input_source_git::file(const boost::optional<std::string> & path_one, const void * context_original,
                                    const boost::optional<std::string> & path_two, const void * context_modified) {

  std::string path_original = path_one ? *path_one : "";
  std::string path_modified = path_two ? *path_two : "";

  if(srcml_archive_check_extension(options.archive, path_original.c_str()) == SRCML_LANGUAGE_NONE
    && srcml_archive_check_extension(options.archive, path_modified.c_str()) == SRCML_LANGUAGE_NONE)
    return;

  boost::optional<std::string> path = path_one;
  if(!path || path->empty()) path = path_two;

  const std::string language_string = srcml_archive_check_extension(options.archive, path->c_str());

  std::string unit_filename = !path_original.empty() ? path_original.substr(directory_length_original) : std::string();
  std::string filename_two  = !path_modified.empty() ? path_modified.substr(directory_length_modified) : std::string();
  if(path_modified.empty() || unit_filename != filename_two) {

    unit_filename += '|';
    unit_filename += filename_two;

  }

  const git_oid * blob_oid_original = (const git_oid *)context_original;
  const git_oid * blob_oid_modified = (const git_oid *)context_modified;

  char * buf_original = new char[GIT_OID_HEXSZ + 1];
  path_original += '@';
  path_original += git_oid_tostr(buf_original, GIT_OID_HEXSZ + 1, blob_oid_original);
  if(buf_original) delete buf_original;

  char * buf_modified = new char[GIT_OID_HEXSZ + 1];
  path_modified += '@';
  path_modified += git_oid_tostr(buf_modified, GIT_OID_HEXSZ + 1, blob_oid_modified);
  if(buf_modified) delete buf_modified;

  srcdiff_input_git input_original(options.archive, path_original, 0, *this);
  srcdiff_input_git input_modified(options.archive, path_modified, 0, *this);

  LineDiffRange line_diff_range(path_original, path_modified, boost::optional<std::string>(), clone_path.native());

  translator->translate(input_original, input_modified, line_diff_range, language_string, NULL, unit_filename, 0);

}

bool operator<(const std::pair<std::string, size_t> & pair_one, const std::pair<std::string, size_t> & pair_two) {

  return pair_one.first < pair_two.first;

}

void srcdiff_input_source_git::directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                         const boost::optional<std::string> & directory_modified, const void * context_modified) {

#ifdef __MINGW32__
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

  const git_tree * current_tree_original = (const git_tree *)context_original;
  const git_tree * current_tree_modified = (const git_tree *)context_modified;

  std::string path_original;
  int basesize_original = 0;
  if(directory_original) {

    path_original = *directory_original;
    if (!path_original.empty() && path_original.back() != PATH_SEPARATOR) path_original += PATH_SEPARATOR;
    basesize_original = path_original.size();

  }

  std::string path_modified;
  int basesize_modified = 0;
  if(directory_modified) {

    path_modified = *directory_modified;
    if (!path_modified.empty() && path_modified.back() != PATH_SEPARATOR) path_modified += PATH_SEPARATOR;
    basesize_modified = path_modified.size();

  }

  size_t count_original = git_tree_entrycount(current_tree_original);
  std::vector<std::pair<std::string, size_t>> names_original;
  names_original.reserve(count_original);
  for(size_t i = 0; i < count_original; ++i) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, i);
    if (entry_original) names_original.push_back(std::make_pair(path_original.replace(basesize_original, std::string::npos, git_tree_entry_name(entry_original)), i));

  }
  sort(names_original.begin(), names_original.end(), operator<);

  size_t count_modified = git_tree_entrycount(current_tree_modified);
  std::vector<std::pair<std::string, size_t>> names_modified;
  names_modified.reserve(count_modified);
  for(size_t i = 0; i < count_modified; ++i) {

    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, i);
    if (entry_modified) names_modified.push_back(std::make_pair(path_modified.replace(basesize_modified, std::string::npos, git_tree_entry_name(entry_modified)), i));

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

    boost::optional<std::string> file_path_original;
    boost::optional<std::string> file_path_modified;
    const git_oid * blob_oid_original = 0;
    const git_oid * blob_oid_modified = 0;
    
    if(comparison <= 0) file_path_original = names_original.at(pos_original).first, blob_oid_original = git_tree_entry_id(entry_original), ++pos_original;
    if(comparison >= 0) file_path_modified = names_modified.at(pos_modified).first, blob_oid_modified = git_tree_entry_id(entry_modified), ++pos_modified;

    file(file_path_original, blob_oid_original, file_path_modified, blob_oid_modified);

  }

  while(pos_original < count_original) {

    git_tree_entry * entry_original = (git_tree_entry *)git_tree_entry_byindex(current_tree_original, names_original.at(pos_original).second);
    if(!entry_original || git_tree_entry_type(entry_original) == GIT_OBJ_TREE) { ++pos_original; continue; }

    boost::optional<std::string> file_path_original = names_original.at(pos_original).first;
    const git_oid * blob_oid_original = git_tree_entry_id(entry_original);
    file(file_path_original, blob_oid_original, boost::optional<std::string>(), 0);

    ++pos_original;

  }

  while(pos_modified < count_modified) {

    git_tree_entry * entry_modified = (git_tree_entry *)git_tree_entry_byindex(current_tree_modified, names_modified.at(pos_modified).second);
    if(!entry_modified || git_tree_entry_type(entry_modified) == GIT_OBJ_TREE) { ++pos_modified; continue; }

    boost::optional<std::string> file_path_modified = names_modified.at(pos_modified).first;
    const git_oid * blob_oid_modified = git_tree_entry_id(entry_modified);
    file(boost::optional<std::string>(), 0, file_path_modified, blob_oid_modified);

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

    directory(path_original, subtree_original, path_modified, subtree_modified);

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

    directory(path_original, subtree_original, boost::optional<std::string>(), nullptr);

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

    directory(boost::optional<std::string>(), nullptr, path_modified, subtree_modified);

    if(subtree_modified) git_tree_free(subtree_modified);

    ++pos_modified;

  }

#undef PATH_SEPARATOR

}

void srcdiff_input_source_git::files_from() {}

srcdiff_input_source_git::git_context * srcdiff_input_source_git::open(const char * uri) const {

  git_context * context = new git_context;

  char * at_pos = index(uri, '@');
  const std::string path(uri, at_pos - uri);
  std::string oid_str(at_pos + 1);

  git_oid oid = { 0 };

  mutex.lock();
  git_oid_fromstr(&oid, oid_str.c_str());

  git_blob_lookup(&context->blob, repo, &oid);

  context->content = { 0 };

  int error = git_blob_filtered_content(&context->content, context->blob, path.c_str(), true);
  mutex.unlock();

  context->pos = 0;

  return context;

}

int srcdiff_input_source_git::read(void * context, char * buffer, int len) {

  git_context * ctx = (git_context *)context;

  int remaining = ctx->content.size - ctx->pos;

  int length = remaining >= len ? len : remaining;

  memcpy((void *)buffer, (void *)(ctx->content.ptr + ctx->pos), length);

  ctx->pos += length;

  return length;

}

int srcdiff_input_source_git::close(void * context) {

  git_context * ctx = (git_context *)context;

  git_buf_free(&ctx->content);

  git_blob_free(ctx->blob);

  delete ctx;

  return 1;

}
