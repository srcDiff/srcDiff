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

void input_source_manager::append_source(std::unique_ptr<input_source> input) {
    input_sources.push_back(std::move(input));
}

input_source_manager::operator bool(){
	return input_sources.size() > 1;
}

void input_source_manager::consume() {
	assert(bool(*this));

	// first source is original/second is modified
	// check if more and put in while, and
	// add error handling, correction, directory, concurrent, possibly separate input streams, parallelism
	stream_manager.append_stream(std::move(input_sources.front()->next()));
	input_sources.pop_front();

	stream_manager.append_stream(std::move(input_sources.front()->next()));
	input_sources.pop_front();

	// fix these
  	std::string unit_filename = "";
  	std::string language_string = "C++";

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
