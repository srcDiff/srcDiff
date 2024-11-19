// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_input_source.cpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcdiff_input_source.hpp>

#include <unified_view.hpp>
#include <side_by_side_view.hpp>

bool srcdiff_input_source::show_input = false;

size_t srcdiff_input_source::input_count = 0;
size_t srcdiff_input_source::input_skipped = 0;
size_t srcdiff_input_source::input_total = 0;

srcdiff_input_source::srcdiff_input_source(const srcdiff_options & options) : options(options), translator(), view(), directory_length_original(0), directory_length_modified(0) {

  OPTION_TYPE flags = options.flags;
　if(srcml_archive_get_version(options.archive)
    && srcml_archive_is_solitary_unit(options.archive)) {

    unit_version = srcml_archive_get_version(options.archive);

  }

  show_input = is_option(flags, OPTION_VERBOSE) && !is_option(flags, OPTION_QUIET);

  const srcdiff_options::view_options_t& view_options = options.view_options;

  translator = std::make_unique<srcdiff_translator>(
                options.srcdiff_filename, options.flags, options.methods, options.archive,
                options.unit_filename,
                options.view_options,
                options.summary_type_str);

  if(is_option(flags, OPTION_UNIFIED_VIEW)) {

     view = std::make_unique<unified_view>(
              options.srcdiff_filename,
              view_options.syntax_highlight,
              view_options.theme,
              is_option(flags, OPTION_IGNORE_ALL_WHITESPACE),
              is_option(flags, OPTION_IGNORE_WHITESPACE),
              is_option(flags, OPTION_IGNORE_COMMENTS),
              is_option(flags, OPTION_HTML_VIEW),
              view_options.unified_view_context);

  } else if(is_option(flags, OPTION_SIDE_BY_SIDE_VIEW)) {

     view = std::make_unique<side_by_side_view>(
              options.srcdiff_filename,
              view_options.syntax_highlight,
              view_options.theme,
              is_option(flags, OPTION_IGNORE_ALL_WHITESPACE),
              is_option(flags, OPTION_IGNORE_WHITESPACE),
              is_option(flags, OPTION_IGNORE_COMMENTS),
              is_option(flags, OPTION_HTML_VIEW),
              view_options.side_by_side_tab_size);

  }

}

srcdiff_input_source::~srcdiff_input_source() {
}

void srcdiff_input_source::file(const std::optional<std::string> & path_original,
                                const std::optional<std::string> & path_modified) {

  if(show_input) {

    ++input_total;

    if(get_language(path_original, path_modified) == SRCML_LANGUAGE_NONE) {

      ++input_skipped;
      std::cout << "- " << (path_original ? *path_original : "") << '|' << (path_modified ? *path_modified : "") << '\n';

    } else {

      ++input_count;
      std::cout << input_count << " " << (path_original ? *path_original : "") << '|' << (path_modified ? *path_modified : "") << '\n';

    }

  }

  std::string srcdiff = process_file(path_original, path_modified);

  if(!view) {
    translator->write_translation();
  } else {
    view->transform(srcdiff, "UTF-8");
  }

}

void srcdiff_input_source::directory(const std::optional<std::string> & directory_original,
                                     const std::optional<std::string> & directory_modified) {

  show_input = !is_option(options.flags, OPTION_QUIET);
  srcml_archive_disable_solitary_unit(options.archive);

  if(show_input) {

    ++input_skipped;
    ++input_total;
    std::cout << "- " << (directory_original ? *directory_original : "") << '|' << (directory_modified ? *directory_modified : "") << '\n';

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

    std::cout << "Processing files from:  " << *options.files_from_name << '\n';

  }

  process_files_from();

}

const char * srcdiff_input_source::get_language(const std::optional<std::string> & path_original, const std::optional<std::string> & path_modified) {

  const char * archive_language = srcml_archive_get_language(options.archive);
  if (archive_language) {
    return archive_language;
  } else {
    std::optional<std::string> path = path_original;
    if(!path || path->empty()) path = path_modified;
    if(!path) path = std::string();

    return srcml_archive_check_extension(options.archive, path->c_str());
  }

}
