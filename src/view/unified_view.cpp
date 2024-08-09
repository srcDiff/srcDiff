// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file unified_view.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <unified_view.hpp>

#include <srcdiff_constants.hpp>

#include <type_query.hpp>

#include <cstring>
#include <cassert>
// signed vs unsigned comparisons, unused parameters, future API usage again?
unified_view::unified_view(const std::string & output_filename,
                           const std::string & syntax_highlight,
                           const std::string & theme,
                           bool ignore_all_whitespace,
                           bool ignore_whitespace,
                           bool ignore_comments,
                           bool is_html,
                           std::any context_type)
              : view_t(output_filename,
                          syntax_highlight, 
                          theme,
                          ignore_all_whitespace,
                          ignore_whitespace,
                          ignore_comments,
                          is_html),
                last_character_operation(view_t::UNSET), modes(LINE), line_number_delete(0), line_number_insert(0), number_context_lines(3),
                is_after_change(false), wait_change(true), in_function(),
                context_type(context_type), length(0), is_after_additional(false),
                after_edit_count(0), last_context_line((unsigned)-1),
                change_starting_line(false), change_ending_space(),
                change_ending_operation(view_t::UNSET),
                close_num_spans(0) {

  if(context_type.type() == typeid(int)) {

    number_context_lines = std::any_cast<int>(context_type);

  } else {

    const std::string & context_type_str = std::any_cast<std::string>(context_type);
    const std::string::size_type dash_pos = context_type_str.find('-');
    context_mode mode = context_string_to_id(context_type_str.substr(0, dash_pos));

    if(mode == ALL) number_context_lines = -1;
    else modes |= mode;

    // assume dash is -only /** @todo actually complete this */
    if(dash_pos != std::string::npos) modes = FUNCTION;

  }

}

unified_view::~unified_view() {}

void unified_view::reset_internal() {

  line_number_delete = 0;
  line_number_insert = 0;
  is_after_change = false;
  wait_change = true;
  in_function.clear();
  length = 0;
  additional_context.clear();
  is_after_additional = false;
  after_edit_count = 0;
  last_context_line = -1;
  last_character_operation = view_t::UNSET;
  change_starting_line = false;
  change_ending_space = "";
  change_ending_operation = view_t::UNSET;
  close_num_spans = 0;

}

unified_view::context_mode unified_view::context_string_to_id(const std::string & context_type_str) const {

  if(context_type_str == "all")      return ALL;
  if(context_type_str == "function") return FUNCTION;
  else                               return LINE;


}

bool unified_view::in_mode(context_mode mode) {

  return mode & modes;

}

void unified_view::output_additional_context() {

  std::size_t line_delete = line_number_delete + 1 - additional_context.size();
  std::size_t line_insert = line_number_insert + 1 - additional_context.size();

  if(number_context_lines != -1 && wait_change && last_context_line != (line_number_delete - 1)) {

    if(!is_html) {
      (*output) << theme->common_color << theme->line_number_color << "@@ -" << line_delete << " +" << line_insert << " @@" << theme->common_color << '\n';
    }
    else {
      (*output) << "<span style=\"color:" << theme->line_number_color << ";\">@@ -" << line_delete << " +" << line_insert << " @@" << "</span>\n";
    }

  }

  if(additional_context.empty()) return;

  for(std::list<std::string>::const_iterator citr = additional_context.begin(); citr != additional_context.end(); ++citr) {

    (*output) << *citr;

    ++line_delete, ++line_insert;

  }

  additional_context.clear();
  length = 0;

}

void unified_view::output_characters(const std::string & ch, int operation) {

  output_characters_to_buffer(*output, ch, operation, last_character_operation, close_num_spans);

}

void unified_view::start_unit(const std::string & local_name [[maybe_unused]],
                              const char * prefix [[maybe_unused]],
                              const char * URI [[maybe_unused]],
                              int num_namespaces [[maybe_unused]],
                              const struct srcsax_namespace * namespaces [[maybe_unused]],
                              int num_attributes [[maybe_unused]],
                              const struct srcsax_attribute * attributes [[maybe_unused]]) {

  if(is_html) {
    (*output) << "<pre>";
  }

}

void unified_view::start_element(const std::string & local_name,
                                 const char * prefix [[maybe_unused]],
                                 const char * URI,
                                 int num_namespaces [[maybe_unused]],
                                 const struct srcsax_namespace * namespaces [[maybe_unused]],
                                 int num_attributes [[maybe_unused]],
                                 const struct srcsax_attribute * attributes [[maybe_unused]]) {


  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(ignore_comments && in_comment) return;

    if(local_name == "common") {
     diff_stack.push_back(view_t::COMMON);
    }
    else if(local_name == "delete") {
     diff_stack.push_back(DELETE);
    }
    else if(local_name == "insert") {
     diff_stack.push_back(INSERT);
    }
    else if(local_name == "ws" && ignore_all_whitespace) {
      diff_stack.push_back(view_t::COMMON);
    }
    
  } else {

    if(local_name == "comment") {

      if(ignore_comments) {
        diff_stack.push_back(view_t::COMMON);
      }

    }

    if(in_mode(FUNCTION) && is_function_type(local_name)) {

      in_function.push_back(true);

      if(in_function.size() == 1) {

        additional_context.clear();
        length = 0;

      }

    }

  }

}

void unified_view::end_unit(const std::string & local_name [[maybe_unused]],
                            const char * prefix [[maybe_unused]],
                            const char * URI [[maybe_unused]]) {

  if(!change_ending_space.empty()) {

    output_characters(change_ending_space, view_t::COMMON);
    change_ending_space = "";
    change_ending_operation = view_t::COMMON;

  }

  if(!(number_context_lines != -1 && wait_change)) {
    output_additional_context();
  }

  if(is_html) {
    (*output) << "</pre>";
  }

}

void unified_view::end_element(const std::string & local_name,
                               const char * prefix [[maybe_unused]],
                               const char * URI) {

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(ignore_comments && in_comment) return;

      if(local_name == "common" || local_name == "delete" || local_name == "insert"
        || (local_name == "ws" && ignore_all_whitespace)) {
        diff_stack.pop_back();
      }

  } else {

    if(local_name == "comment") {

      if(ignore_comments) {
        diff_stack.pop_back();
      }
      
    }

    if(in_mode(FUNCTION) && is_function_type(local_name)) {

      in_function.pop_back();
      if(in_function.size() == 0) {

        additional_context.clear();
        length = 0;

      }

    }

  }

}

void unified_view::characters(const char * ch, int len) {

  if(diff_stack.back() != view_t::COMMON) {

   output_additional_context();

   is_after_additional = false;
   is_after_change = false;
   wait_change = false;

   output->write(context.str().c_str(), context.str().size());
   context.str("");

  }

  if(!change_ending_space.empty() && change_ending_operation != diff_stack.back()) {

    output_characters(change_ending_space, view_t::COMMON);
    change_ending_space = "";
    change_ending_operation = view_t::UNSET;

  }

  if((last_character_operation == view_t::UNSET || last_character_operation == view_t::COMMON)
     && diff_stack.back() != view_t::COMMON && ignore_whitespace) {
    change_starting_line = true;
  }

  for(int i = 0; i < len; ++i) {

    bool skip = false;
    bool is_space = isspace(ch[i]);
    if(is_space) {

      if(ignore_whitespace && diff_stack.back() != view_t::COMMON) {

        if(change_starting_line) {

          output_character(ch[i], view_t::COMMON);

        } else {

          change_ending_space += ch[i];
          change_ending_operation = diff_stack.back();

        }

        skip = true;

      }

    } else {

      if(!change_ending_space.empty()) {

        output_characters(change_ending_space, diff_stack.back());
        change_ending_space = "";
        change_ending_operation = view_t::UNSET;

      }

      change_starting_line = false;

    }

    std::string str(1, ch[i]);
    if(!is_space || ch[i] != '\n') {

      ++i;
      while(i < len && isspace(ch[i]) == is_space && ch[i] != '\n') {

        str += ch[i];
        ++i;

      }

      --i;

    }

    if(wait_change) {

      assert(!skip);
      output_characters_to_buffer(context, str, view_t::COMMON, last_character_operation, close_num_spans);

    } else if(!skip) {

      if(diff_stack.back() != view_t::COMMON && ch[i] == '\n') {

        output_characters(CARRIAGE_RETURN_SYMBOL, diff_stack.back());
        output_character(ch[i], view_t::COMMON);

      } else {

        output_characters(str, diff_stack.back());

      }

    }

    if(ch[i] == '\n') {

      if(!change_ending_space.empty()) {

        output_characters(change_ending_space, view_t::COMMON);
        change_ending_space = "";
        change_ending_operation = view_t::UNSET;

      }

      if(diff_stack.back() != view_t::COMMON) {
        change_starting_line = true;
      }

      if(is_after_change) {

        is_after_change = false;
        is_after_additional = true;

      } else if(is_after_additional) {

        ++after_edit_count;

        if((in_mode(LINE) && (!in_mode(FUNCTION) || !in_function.size()) && after_edit_count == number_context_lines) || (in_mode(FUNCTION) && !in_function.size())) {

          is_after_additional = false;
          after_edit_count = 0;
          wait_change = true;
          last_context_line = line_number_delete;

          end_buffer(context, close_num_spans);
          last_character_operation = view_t::UNSET;      

        }

      } else if(wait_change && ((in_mode(LINE) && (!in_mode(FUNCTION) || !in_function.size()) && number_context_lines != 0) || (in_mode(FUNCTION) && in_function.size()))) {

        if(in_mode(LINE) && (!in_mode(FUNCTION) || !in_function.size()) && length >= number_context_lines) {
          additional_context.pop_front(), --length;
        }

        end_buffer(context, close_num_spans);
        last_character_operation = view_t::UNSET;

        additional_context.push_back(context.str());
        ++length;

      }

      if(diff_stack.back() != INSERT) ++line_number_delete;
      if(diff_stack.back() != DELETE) ++line_number_insert;

      context.str("");

    }

  }

  if(diff_stack.back() != view_t::COMMON) is_after_change  = true;

}
