#include <srcdiff_input_source.hpp>

size_t srcdiff_input_source::input_skipped = 0;

void srcdiff_input_source::file(const boost::optional<std::string> & path_one, const void * context_original,
                                const boost::optional<std::string> & path_two, const void * context_modified) {

  if(is_option(options.flags, OPTION_VERBOSE)) {

    std::cerr << "Processing: " << (path_one ? *path_one : "") << '|' << (path_two ? *path_two : "") << '\n';

  }

  process_file(path_one, context_original, path_two, context_modified);

}

void srcdiff_input_source::directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                     const boost::optional<std::string> & directory_modified, const void * context_modified) {

  if(is_option(options.flags, OPTION_VERBOSE)) {

    std::cerr << "Processing: " << (directory_original ? *directory_original : "") << '|' << (directory_modified ? *directory_modified : "") << '\n';
    ++input_skipped;

  }

  process_directory(directory_original, context_original, directory_modified, context_modified);

}
