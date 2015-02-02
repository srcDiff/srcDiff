#include <srcdiff_input_source.hpp>

bool srcdiff_input_source::show_input = false;

size_t srcdiff_input_source::input_count = 0;
size_t srcdiff_input_source::input_skipped = 0;
size_t srcdiff_input_source::input_total = 0;

void srcdiff_input_source::file(const boost::optional<std::string> & path_one, const void * context_original,
                                const boost::optional<std::string> & path_two, const void * context_modified) {

  if(show_input) {

    ++input_total;

    if(get_language(path_one, path_two) == SRCML_LANGUAGE_NONE) {

      ++input_skipped;
      std::cerr << "- " << (path_one ? *path_one : "") << '|' << (path_two ? *path_two : "") << '\n';

    } else {

      ++input_count;
      std::cerr << input_count << " " << (path_one ? *path_one : "") << '|' << (path_two ? *path_two : "") << '\n';

    }

  }

  process_file(path_one, context_original, path_two, context_modified);

}

void srcdiff_input_source::directory(const boost::optional<std::string> & directory_original, const void * context_original,
                                     const boost::optional<std::string> & directory_modified, const void * context_modified) {

  show_input = !is_option(options.flags, OPTION_QUIET);

  if(show_input) {

    ++input_skipped;
    ++input_total;
    std::cerr << "- " << (directory_original ? *directory_original : "") << '|' << (directory_modified ? *directory_modified : "") << '\n';

  }

  process_directory(directory_original, context_original, directory_modified, context_modified);

}
