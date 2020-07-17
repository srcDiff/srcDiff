#include <srcdiff_input_source.hpp>

bool srcdiff_input_source::show_input = false;

size_t srcdiff_input_source::input_count = 0;
size_t srcdiff_input_source::input_skipped = 0;
size_t srcdiff_input_source::input_total = 0;

srcdiff_input_source::srcdiff_input_source(const srcdiff_options & options) : options(options), translator(0), directory_length_original(0), directory_length_modified(0) {

  if(srcml_archive_get_version(options.archive)
    && (srcml_archive_is_solitary_unit(options.archive)
      || is_option(options.flags, OPTION_BURST))) {

    unit_version = srcml_archive_get_version(options.archive);

  }

  show_input = is_option(options.flags, OPTION_VERBOSE) && !is_option(options.flags, OPTION_QUIET);

}

void srcdiff_input_source::file(const boost::optional<std::string> & path_original,
                                const boost::optional<std::string> & path_modified) {

  if(show_input) {

    ++input_total;

    if(get_language(path_original, path_modified) == SRCML_LANGUAGE_NONE) {

      ++input_skipped;
      std::cerr << "- " << (path_original ? *path_original : "") << '|' << (path_modified ? *path_modified : "") << '\n';

    } else {

      ++input_count;
      std::cerr << input_count << " " << (path_original ? *path_original : "") << '|' << (path_modified ? *path_modified : "") << '\n';

    }

  }

  process_file(path_original, path_modified);

}

void srcdiff_input_source::directory(const boost::optional<std::string> & directory_original,
                                     const boost::optional<std::string> & directory_modified) {

  show_input = !is_option(options.flags, OPTION_QUIET);

  if(show_input) {

    ++input_skipped;
    ++input_total;
    std::cerr << "- " << (directory_original ? *directory_original : "") << '|' << (directory_modified ? *directory_modified : "") << '\n';

  }

  try {

    process_directory(directory_original, directory_modified);

  } catch(const std::string & error) {

    std::cerr << error << '\n';
    exit(1);

  }

}

void srcdiff_input_source::files_from() {

  show_input = !is_option(options.flags, OPTION_QUIET);

  if(show_input) {

    std::cerr << "Processing files from:  " << *options.files_from_name << '\n';

  }

  process_files_from();

}
