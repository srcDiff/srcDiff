// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_source_manager.cpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <input_source_manager.hpp>

#include <unified_view.hpp>
#include <side_by_side_view.hpp>

namespace srcdiff {

input_source_manager::input_source_manager(const client_options& options)
	: options(options), input_sources(), stream_manager(options.is_option(OPTION_STRING_SPLITTING)),
	  deltor(), view()  {
}

void input_source_manager::init() {
	
  deltor = std::make_unique<class deltor>(options.methods);

  const client_options::view_options_t& view_options = options.view_options;
  if(options.is_option(OPTION_UNIFIED_VIEW)) {

     view = std::make_unique<unified_view>(
              options.output_filename,
              view_options.syntax_highlight,
              view_options.theme,
              options.is_option(OPTION_IGNORE_ALL_WHITESPACE),
              options.is_option(OPTION_IGNORE_WHITESPACE),
              options.is_option(OPTION_IGNORE_COMMENTS),
              options.is_option(OPTION_HTML_VIEW),
              view_options.unified_view_context);

  } else if(options.is_option(OPTION_SIDE_BY_SIDE_VIEW)) {

     view = std::make_unique<side_by_side_view>(
              options.output_filename,
              view_options.syntax_highlight,
              view_options.theme,
              options.is_option(OPTION_IGNORE_ALL_WHITESPACE),
              options.is_option(OPTION_IGNORE_WHITESPACE),
              options.is_option(OPTION_IGNORE_COMMENTS),
              options.is_option(OPTION_HTML_VIEW),
              view_options.side_by_side_tab_size);

  }
}

input_source_manager::~input_source_manager() {
}

void input_source_manager::append_source(std::shared_ptr<input_source> input) {
    input_sources.push_back(input);
}

input_source_manager::operator bool(){
	return input_sources.size() > 1;
}

void input_source_manager::consume() {
	assert(bool(*this));

  //   // if we're not at the end of the original contents list, but: we are at the
  //   // end of the modified files list, or in_original is less than the current
  //   // entry in the modified files list, then in_original has no match; process
  //   // it and then go to the next one in its list
  //   if (in_original != entry.end() &&
  //       (in_modified == modified_contents.end() ||
  //       in_original->path().filename() < in_modified->path().filename())) {
  //     file(in_original->path().string(), std::optional<std::string>());
  //     ++in_original;
  //   } else if(in_original == entry.end() || 
  //       in_modified->path().filename() < in_original->path().filename()) {
  //     // similarly, process in_modified if it doesn't match in_original
  //     file(std::optional<std::string>(), in_modified->path().string());
  //     ++in_modified;
  //   } else {
  //     // having dealt with the problematic cases, we can compare two matching files
  //     file(in_original->path().string(), in_modified->path().string());
  //     ++in_original;
  //     ++in_modified;
  //   }
  // }

  // in_original = entry.begin();
  // in_modified = modified_contents.begin();

  // while (in_original != entry.end() || in_modified != modified_contents.end()) {

  //   if (in_original != entry.end() && !in_original->is_directory()) {
  //     ++in_original;
  //     continue;
  //   }

  //   if (in_modified != modified_contents.end() && !in_modified->is_directory()) {
  //     ++in_modified;
  //     continue;
  //   }

  //   // same logic as processing files
  //   if (in_original != entry.end() &&
  //       (in_modified == modified_contents.end() ||
  //       in_original->path().filename() < in_modified->path().filename())) {
  //     directory(in_original->path().string(), std::optional<std::string>());
  //     ++in_original;
  //   } else if(in_original == entry.end() || 
  //       in_modified->path().filename() < in_original->path().filename()) {
  //     directory(std::optional<std::string>(), in_modified->path().string());
  //     ++in_modified;
  //   } else {
  //     // having dealt with the problematic cases, we can compare two matching
  //     // directories
  //     directory(
  //       in_original->path().string(),
  //       in_modified->path().string()
  //     );
  //     ++in_original;
  //     ++in_modified;
  //   }
  // }

  // if(show_input) {

  //   ++input_skipped;
  //   ++input_total;
  //   std::cout << "- " << (directory_original ? *directory_original : "") << '|' << (directory_modified ? *directory_modified : "") << '\n';

  // }

	// first source is original/second is modified
	// check if more and put in while, and
	// add error handling, correction, directory, concurrent, possibly separate input streams, parallelism
	std::shared_ptr<input_source> original_source = input_sources.front();
	input_sources.pop_front();

	std::filesystem::directory_entry entry = original_source->entry();
	while(entry.is_directory()) {
		original_source->next();
		entry = original_source->entry();
	}

	stream_manager.append_original_stream(original_source->stream());
	original_source->next();

	std::shared_ptr<input_source> modified_source = input_sources.front();
	input_sources.pop_front();

	entry = modified_source->entry();
	while(entry.is_directory()) {
		modified_source->next();
		entry = modified_source->entry();
	}

	stream_manager.append_modified_stream(modified_source->stream());
	modified_source->next();

	if(*modified_source) input_sources.push_front(modified_source);
	if(*original_source) input_sources.push_front(original_source);

	srcml_unit* srcdiff_unit = deltor->create(options.archive, stream_manager);

	if(!view) {
	    srcml_archive_write_unit(options.archive, srcdiff_unit);
  	} else {
	    std::string srcdiff = srcml_unit_get_srcml(srcdiff_unit);
	    view->transform(srcdiff, "UTF-8");
	  }

	srcml_unit_free(srcdiff_unit);

}

}
