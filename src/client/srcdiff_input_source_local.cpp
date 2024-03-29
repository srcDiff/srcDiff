#include <srcdiff_input_source_local.hpp>

#include <srcml.h>

#include <srcdiff_input.hpp>

#include <uri_stream.hpp>

#include <cstring>
#include <filesystem>

// file/directory names to ignore when processing a directory
// const/non-const versions for linux/bsd different declarations
// int srcdiff_input_source_local::dir_filter(std::filesystem::path d) {

//     return d->d_name[0] != '.';
// }

int srcdiff_input_source_local::is_dir(std::filesystem::directory_entry d) {
  return d.is_directory();
}

// bool srcdiff_input_source_local::is_output_file(const char * filename, const struct stat & outstat) {

//   struct stat instat = { 0 };

//   int stat_status = stat(filename, &instat);

//   if(stat_status)
//     return stat_status;

//   if(instat.st_ino == outstat.st_ino && instat.st_dev == outstat.st_dev)
//     return 1;

//   return 0;

// }

srcdiff_input_source_local::srcdiff_input_source_local(const srcdiff_options & options) : srcdiff_input_source(options) {

  translator = new srcdiff_translator(options.srcdiff_filename, options.flags, options.methods, options.archive,
                                      options.unit_filename,
                                      options.view_options,
                                      options.summary_type_str);

  output_file = std::filesystem::directory_entry(options.srcdiff_filename);

}

srcdiff_input_source_local::~srcdiff_input_source_local() {

  if(translator) delete translator;

}

// determines whether the input path(s) exist and whether they are files or
// directories, and then processes them
void srcdiff_input_source_local::consume()
{

  if (options.files_from_name) {
    
    files_from();
  
  } else {

    for (std::pair<std::string, std::string> input_pair : options.input_pairs) {

      std::filesystem::path original(input_pair.first);
      std::filesystem::path modified(input_pair.second);

      if (!std::filesystem::exists(original) && !std::filesystem::exists(modified))
        throw std::string("Input sources '" + input_pair.first + "' and '" + input_pair.second + "' could not be opened");
      else if (!std::filesystem::exists(original))
        throw std::string("Input source '" + input_pair.first + "' could not be opened");
      else if (!std::filesystem::exists(modified))
        throw std::string("Input source '" + input_pair.second + "' could not be opened");

      if (std::filesystem::is_directory(original)) {

        srcml_archive_enable_solitary_unit(options.archive);

        if (!srcml_archive_get_url(options.archive))
        {

          std::string directory_path = input_pair.first == input_pair.second ? input_pair.first : input_pair.first + '|' + input_pair.second;
          srcml_archive_set_url(options.archive, directory_path.c_str());
        }

        directory_length_original = input_pair.first.back() == '/' ? input_pair.first.size() : input_pair.first.size() + 1;
        directory_length_modified = input_pair.second.back() == '/' ? input_pair.second.size() : input_pair.second.size() + 1;

        directory(input_pair.first, input_pair.second);
      } else {
        file(input_pair.first, input_pair.second);
      }
    }
  }
}

const char * srcdiff_input_source_local::get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified) {

  std::optional<std::string> path = path_original;
  if(!path || path->empty()) path = path_modified;
  if(!path) path = std::string();

  return srcml_archive_check_extension(options.archive, path->c_str());

}

void srcdiff_input_source_local::process_file(const std::optional<std::string> & path_original,
                                              const std::optional<std::string> & path_modified) {

  const char * language_string = get_language(path_original, path_modified);

  if(language_string == SRCML_LANGUAGE_NONE) return;

  std::string path_one = path_original ? *path_original : std::string();
  std::string path_two = path_modified ? *path_modified : std::string();

  std::string unit_filename = !path_one.empty() ? path_one.substr(directory_length_original) : path_one;
  std::string filename_two =  !path_two.empty() ? path_two.substr(directory_length_modified) : path_two;
  if(path_two.empty() || unit_filename != filename_two) {

    unit_filename += "|";
    unit_filename += filename_two;

  }

  srcdiff_input<srcdiff_input_source_local> input_original(options.archive, path_original, language_string, options.flags, *this);
  srcdiff_input<srcdiff_input_source_local> input_modified(options.archive, path_modified, language_string, options.flags, *this);
  translator->translate(input_original, input_modified, language_string, unit_filename, unit_version);

}

void srcdiff_input_source_local::process_directory(const std::optional<std::string> & directory_original,
                                                   const std::optional<std::string> & directory_modified) {

  std::filesystem::directory_entry original_entry(directory_original ? *directory_original : "");
  std::filesystem::directory_entry modified_entry(directory_modified ? *directory_modified : "");

  if (!original_entry.is_directory() && !modified_entry.is_directory()) {

    throw std::string("Directories '" + (directory_original ? *directory_original : "")
      + "' and '" + (directory_modified ? *directory_modified : "") + "' could not be opened");

  } else if(!original_entry.is_directory()) {

    throw std::string("Directory '" + (directory_original ? *directory_original : "") + "' could not be opened");
  
  } else if(!original_entry.is_directory()){
  
    throw std::string("Directory '" + (directory_modified ? *directory_modified : "") + "' could not be opened");
  
  }

  std::vector<std::filesystem::directory_entry> original_contents;
  for (auto e : std::filesystem::directory_iterator(original_entry)){
    original_contents.push_back(e);
  }
  std::sort(original_contents.begin(), original_contents.end());
  
  std::vector<std::filesystem::directory_entry> modified_contents;
  for (auto e : std::filesystem::directory_iterator(modified_entry)){
    modified_contents.push_back(e);
  }
  std::sort(modified_contents.begin(), modified_contents.end());

  // process all non-directory files
  std::vector<std::filesystem::directory_entry>::iterator in_original = original_contents.begin();
  std::vector<std::filesystem::directory_entry>::iterator in_modified = modified_contents.begin();
  while (in_original != original_contents.end() || in_modified != modified_contents.end()) {

    // if we're not at the end of each file list, check to make sure the current
    // entry in the file list is not a directory and is not our output file;
    // skip the current entry otherwise
    if(in_original != original_contents.end() &&
        (in_original->is_directory() || *in_original == output_file)) {
      ++in_original;
      continue;
    }

    if(in_modified != modified_contents.end() &&
        (in_modified->is_directory() || *in_modified == output_file)) {
      ++in_modified;
      continue;
    }

    // if we're not at the end of the original contents list, but: we are at the
    // end of the modified files list, or in_original is less than the current
    // entry in the modified files list, then in_original has no match; process
    // it and then go to the next one in its list
    if (in_original != original_contents.end() &&
        (in_modified == modified_contents.end() ||
        in_original->path().filename() < in_modified->path().filename())) {
      file(in_original->path().string(), std::optional<std::string>());
      ++in_original;
    } else if(in_original == original_contents.end() || 
        in_modified->path().filename() < in_original->path().filename()) {
      // similarly, process in_modified if it doesn't match in_original
      file(std::optional<std::string>(), in_modified->path().string());
      ++in_modified;
    } else {
      // having dealt with the problematic cases, we can compare two matching files
      file(in_original->path().string(), in_modified->path().string());
      ++in_original;
      ++in_modified;
    }
  }

  in_original = original_contents.begin();
  in_modified = modified_contents.begin();

  while (in_original != original_contents.end() || in_modified != modified_contents.end()) {

    if (in_original != original_contents.end() && !in_original->is_directory()) {
      ++in_original;
      continue;
    }

    if (in_modified != modified_contents.end() && !in_modified->is_directory()) {
      ++in_modified;
      continue;
    }

    // same logic as processing files
    if (in_original != original_contents.end() &&
        (in_modified == modified_contents.end() ||
        in_original->path().filename() < in_modified->path().filename())) {
      directory(in_original->path().string(), std::optional<std::string>());
      ++in_original;
    } else if(in_original == original_contents.end() || 
        in_modified->path().filename() < in_original->path().filename()) {
      directory(std::optional<std::string>(), in_modified->path().string());
      ++in_modified;
    } else {
      // having dealt with the problematic cases, we can compare two matching
      // directories
      directory(
        in_original->path().string(),
        in_modified->path().string()
      );
      ++in_original;
      ++in_modified;
    }
  }
}


void srcdiff_input_source_local::process_files_from() {

#define FILELIST_COMMENT '#'

  try {

    // translate all the filenames listed in the named file

    input_context * context = open(options.files_from_name->c_str());
    uri_stream<srcdiff_input_source_local> uriinput(context);

    const char * c_line = 0;
    while ((c_line = uriinput.readline())) {

      std::string line = c_line;
 
      int white_length = strspn(line.c_str(), " \t\f");
      line.erase(0, white_length);

      // skip blank lines or comment lines
      if (line.empty() || line[0] == FILELIST_COMMENT) continue;

      std::string path_original = line.substr(0, line.find('|'));
      std::string path_modified = line.substr(line.find('|') + 1);

      file(path_original, path_modified);

    }

  } catch (uri_stream_error) {

    fprintf(stderr, "error: file/URI \'%s\' does not exist.\n", options.files_from_name->c_str());
    exit(EXIT_FAILURE);

  }

#undef FILELIST_COMMENT

}

srcdiff_input_source_local::input_context * srcdiff_input_source_local::open(const char * uri) const {

  input_context * context = new input_context;

  context->in.open(uri);

  return context->in ? context : (delete context, nullptr);

}

ssize_t srcdiff_input_source_local::read(void * context, void * buffer, size_t len) {

  input_context * ctx = (input_context *)context;

  ctx->in.read((char *)buffer, len);

  return ctx->in.gcount();
}

int srcdiff_input_source_local::close(void * context) {

  input_context * ctx = (input_context *)context;

  ctx->in.close();

  delete ctx;

  return 1;
}
