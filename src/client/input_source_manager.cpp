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
     show_input(false), input_count(0), input_skipped(0), input_total(0),
     deltor(), view()  {
}

void input_source_manager::init() {
   
   show_input = options.is_option(OPTION_VERBOSE) && !options.is_option(OPTION_QUIET);

   if(input_sources.size() > 2 || input_sources.front()->entry().is_directory()) {
      // may need to check second source
      srcml_archive_disable_solitary_unit(options.archive);
      show_input = true;
   }

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

   // first source is original/second is modified
   // check if more and put in while, and
   // add error handling, correction, directory, concurrent, possibly separate input streams, parallelism
   std::shared_ptr<input_source>    original_source = input_sources.front();
   std::filesystem::directory_entry original_entry  = original_source? original_source->entry() : std::filesystem::directory_entry();
   input_sources.pop_front();

   std::shared_ptr<input_source>    modified_source = input_sources.front();
   std::filesystem::directory_entry modified_entry  = modified_source? modified_source->entry() : std::filesystem::directory_entry();
   input_sources.pop_front();

   // if exausted source is_directory is false
   while(original_entry.is_directory() && modified_entry.is_directory()) {

      std::string original_subpath = original_entry.path().lexically_relative(original_source->get_base_path());
      std::string modified_subpath = modified_entry.path().lexically_relative(modified_source->get_base_path());

      std::string original_input_str;
      std::string modified_input_str;
      if(original_subpath == modified_subpath) {
         original_input_str = original_entry.path().native();
         modified_input_str = modified_entry.path().native();

         original_source->next();
         original_entry = original_source->entry();

         modified_source->next();
         modified_entry = modified_source->entry();

      } else if(original_entry.path() < modified_entry.path()) {
         original_input_str = original_entry.path().native();
      
         original_source->next();
         original_entry = original_source->entry();

      } else {
         modified_input_str = modified_entry.path().native();

         modified_source->next();
         modified_entry = modified_source->entry();
      }

      if(show_input) {
         ++input_skipped;
         ++input_total;
         std::cout << "- " << original_input_str << '|' << modified_input_str << '\n';
      }

   }

   std::string original_subpath = original_entry.path().lexically_relative(original_source->get_base_path());
   std::string modified_subpath = modified_entry.path().lexically_relative(modified_source->get_base_path());
   std::string original_input_str;
   std::string modified_input_str;
   const char* language = srcml_archive_get_language(options.archive);
   if(original_subpath == modified_subpath) {
      original_input_str = original_entry.path().native();
      modified_input_str = modified_entry.path().native();

      language = !language? language : srcml_archive_check_extension(options.archive, original_input_str.c_str());
   } else if(!modified_entry.exists() && original_entry.path() < modified_entry.path()) {
      original_input_str = original_entry.path().native();
      language = !language? language : srcml_archive_check_extension(options.archive, original_input_str.c_str());
   } else {
      modified_input_str = modified_entry.path().native();
      language = !language? language : srcml_archive_check_extension(options.archive, modified_input_str.c_str());
   }

   if(show_input) {
      if(!language) {
         ++input_skipped;
         std::cout << "- " << original_input_str << '|' << modified_input_str << '\n';
      } else {
         ++input_count;
         std::cout << input_count << " " << original_input_str << '|' << modified_input_str << '\n';
      }
   }

   stream_manager.append_original_stream(original_source->stream());
   original_source->next();

   stream_manager.append_modified_stream(modified_source->stream());
   modified_source->next();

   // keep both on as long as a one still has streams
   if(*original_source || *modified_source) {
      input_sources.push_front(modified_source);
      input_sources.push_front(original_source);
   }

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
