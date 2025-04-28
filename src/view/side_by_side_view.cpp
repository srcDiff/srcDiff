// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file side_by_side_view.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <side_by_side_view.hpp>

#include <constants.hpp>

#include <algorithm>

#include <cstring>
#include <cassert>
#include <iomanip>

side_by_side_view::side_by_side_view(const std::string & output_filename,
                                     const std::string & syntax_highlight,
                                     const std::string & theme,
                                     bool ignore_all_whitespace,
                                     bool ignore_whitespace,
                                     bool ignore_comments,
                                     bool is_html,
                                     int side_by_side_tab_size)
  : view_t(output_filename,
              syntax_highlight,
              theme,
              ignore_all_whitespace,
              ignore_whitespace,
              ignore_comments,
              is_html),
    side_by_side_tab_size(side_by_side_tab_size),
    line_operations(),
    last_character_operation_original(view_t::COMMON),
    original_lines(),
    last_character_operation_modified(view_t::COMMON),
    modified_lines() {}

side_by_side_view::~side_by_side_view() {}

void side_by_side_view::reset_internal() {

  line_operations.clear();

  last_character_operation_original = view_t::COMMON;
  original_lines.clear();

  last_character_operation_modified = view_t::COMMON;
  modified_lines.clear();

  change_starting_line_original = true;
  change_starting_line_modified = true;

  change_ending_space_original.clear();
  change_ending_space_modified.clear();

}

void side_by_side_view::output_characters(const std::string & ch, int operation) {

  if(original_lines.empty()) add_new_line();

  int real_operation = operation;
  bool is_ignore = false;
  if(!ch.empty() && isspace(ch[0])) {

    if(ignore_all_whitespace) {
      real_operation = view_t::COMMON;
    }

    if(ignore_all_whitespace || ignore_whitespace) {
      is_ignore = true;
    }

  }

  if(ignore_comments && in_comment) {

    real_operation = view_t::COMMON;
    is_ignore = true;

  }

  int size = ch == CARRIAGE_RETURN_SYMBOL ? 1 : ch.size();

  if(operation != view_t::INSERT) {

    if(change_starting_line_original && !ch.empty() && !isspace(ch[0])) {
      change_starting_line_original = false;
    }

    if(ignore_whitespace && change_starting_line_original
       && operation != view_t::COMMON) {
      real_operation = view_t::COMMON;
    }

    output_characters_to_buffer(*std::get<STREAM>(original_lines.back()),
                                ch, real_operation, last_character_operation_original,
                                std::get<CLOSE_SPANS>(original_lines.back()));
    if(!save_name) {
      std::get<OPERATION>(original_lines.back()) += size;
    }

  }

  if(operation != view_t::DELETE) {

    if(change_starting_line_modified && !ch.empty() && !isspace(ch[0])) {
      change_starting_line_modified = false;
    }

    if(ignore_whitespace && change_starting_line_modified
       && operation != view_t::COMMON) {
      real_operation = view_t::COMMON;
    }

    output_characters_to_buffer(*std::get<STREAM>(modified_lines.back()),
                                ch, real_operation, last_character_operation_modified,
                                std::get<CLOSE_SPANS>(modified_lines.back()));
    if(!save_name) {
      std::get<OPERATION>(modified_lines.back()) += size;
    }

  }

  if(!is_ignore) {
    line_operations.back() |= operation;
  }

}

void side_by_side_view::add_new_line() {

  // either both are empty or they are not
  if(!original_lines.empty()) {

    end_buffer(*std::get<STREAM>(original_lines.back()), std::get<CLOSE_SPANS>(original_lines.back()));
    end_buffer(*std::get<STREAM>(modified_lines.back()), std::get<CLOSE_SPANS>(modified_lines.back()));

  }

  last_character_operation_original = view_t::UNSET;
  original_lines.emplace_back(std::make_shared<std::ostringstream>(), view_t::UNSET, 0, 0);

  last_character_operation_modified = view_t::UNSET;
  modified_lines.emplace_back(std::make_shared<std::ostringstream>(), view_t::UNSET, 0, 0);

  line_operations.push_back(0);

  change_starting_line_original = true;
  change_starting_line_modified = true;

}

void side_by_side_view::output_html() {

  int magnitude = 0;
  std::size_t lines = line_operations.size();
  while(lines > 0) {

    lines /= 10;
    ++magnitude;

  }

  (*output) << "<div><table style=\"border-collapse: collapse;\"><tr><td style=\"border: 1px solid " + theme->text_color + ";\">";

  (*output) << "<table><tr><th><strong>Original</strong></th></tr><tr><td><pre>";

  int line_number = 1;
  for(std::size_t i = 0; i < original_lines.size(); ++i) {

    (*output) << "<span style=\"color: " + theme->line_number_color + ";\">";
    if(line_operations[i] != view_t::INSERT) {

      (*output) << std::right << std::setw(magnitude) << std::setfill(' ') << line_number << ' ';

    } else {

      (*output) << std::string(magnitude + 1, ' ');

    }
    line_number += std::get<LINE_INCR>(original_lines[i]);
    (*output) << "</span>";

    (*output) << std::get<STREAM>(original_lines[i])->str();
    (*output) << theme->common_color << "\n</span>";  

  }
  (*output) << "</pre></td></tr></table></td>";

  (*output) << "<td style=\"border: 1px solid  " + theme->text_color + ";\">";
  (*output) << "<table><tr><th><strong>Modified</strong></th></tr><tr><td><pre>";

  line_number = 1;
  for(std::size_t i = 0; i < modified_lines.size(); ++i) {

    (*output) << "<span style=\"color: " + theme->line_number_color + ";\">";
    if(line_operations[i] != view_t::DELETE) {

      (*output) << std::right << std::setw(magnitude) << std::setfill(' ') << line_number << ' ';

    } else {

      (*output) << std::string(magnitude + 1, ' ');

    }
    line_number += std::get<LINE_INCR>(modified_lines[i]);
    (*output) << "</span>";

    (*output) << std::get<STREAM>(modified_lines[i])->str();
    (*output) << theme->common_color << "\n</span>";  

  }
  (*output) << "</pre></td></tr></table>";

  (*output) << "</td></tr></table></div>";

}

void side_by_side_view::output_bash() {

    int max_width = 0;
    for(const std::tuple<std::shared_ptr<std::ostringstream>, int, unsigned int, std::size_t> & line : original_lines) {
      max_width = std::max(max_width, std::get<OPERATION>(line));
    }

    for(std::size_t i = 0; i < original_lines.size(); ++i) {

      (*output) << theme->common_color << std::get<STREAM>(original_lines[i])->str();

      std::string fill(max_width - std::get<OPERATION>(original_lines[i]), ' ');
      (*output) << theme->common_color << fill;

      if(line_operations[i] == view_t::DELETE) {
        (*output) << " < ";
      }
      else if(line_operations[i] == view_t::INSERT) {
        (*output) << " > ";
      }
      else if(line_operations[i] == view_t::COMMON) {
        (*output) << "   ";
      }
      else if(line_operations[i] == 0) {
        (*output) << "   ";
      }
      else {
        (*output) << " | ";
      }

      (*output) << std::get<STREAM>(modified_lines[i])->str();

      (*output) << theme->common_color << '\n';

    }

}

void side_by_side_view::start_element(const std::string & local_name, 
                                         const char * prefix [[maybe_unused]], const char * URI,
                                         int num_namespaces [[maybe_unused]],
                                         const struct srcsax_namespace * namespaces [[maybe_unused]],
                                         int num_attributes [[maybe_unused]],
                                         const struct srcsax_attribute * attributes [[maybe_unused]]) {

  if(URI == srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common") {
     diff_stack.push_back(view_t::COMMON);
    }
    else if(local_name == "delete") {
     diff_stack.push_back(view_t::DELETE);
    }
    else if(local_name == "insert") {
     diff_stack.push_back(view_t::INSERT);
    }

    if(local_name != "ws") {

      if(diff_stack.back() != view_t::INSERT) {
        change_starting_line_original = true;
      }
   
      if(diff_stack.back() != view_t::DELETE) {
        change_starting_line_modified = true;
      }

    }
      
  }

}

void side_by_side_view::end_element(const std::string & local_name, const char * prefix [[maybe_unused]],
                               const char * URI) {

  if(URI == srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common" || local_name == "delete" || local_name == "insert") {
      diff_stack.pop_back();
    }

  }

}

void side_by_side_view::characters(const char * ch, int len) {

  if((!change_ending_space_original.empty() || !change_ending_space_modified.empty())) {

    assert(change_ending_space_original.empty() || change_ending_space_modified.empty());
    if(!change_ending_space_original.empty() && diff_stack.back() != view_t::DELETE) {

      output_characters(change_ending_space_original, view_t::COMMON);
      change_ending_space_original = "";

    }

    if(!change_ending_space_modified.empty() && diff_stack.back() != view_t::INSERT) {

      output_characters(change_ending_space_modified, view_t::COMMON);
      change_ending_space_modified = "";

    }

  }

  for(int i = 0; i < len; ++i) {

    if(ch[i] == '\n') {

      if(!ignore_all_whitespace && !ignore_whitespace) {

        if(diff_stack.back() != view_t::COMMON) {
          output_characters(CARRIAGE_RETURN_SYMBOL, diff_stack.back());
        }
        else {
          line_operations.back() |= view_t::COMMON;
        }

      } else {

        line_operations.back() |= diff_stack.back();

      }

      if(diff_stack.back() != view_t::INSERT) {
        ++std::get<LINE_INCR>(original_lines.back());
      }

      if(diff_stack.back() != view_t::DELETE) {
        ++std::get<LINE_INCR>(modified_lines.back());
      }

      add_new_line();
      continue;

    }

    if(isspace(ch[i]) && ignore_whitespace && diff_stack.back() != view_t::COMMON) {

      std::string str;
      do {

        if(ch[i] == '\t') {
          str.append(side_by_side_tab_size, ' ');
        }
        else {
          str.append(1, ch[i]);
        }

        ++i;

      } while(i < len && isspace(ch[i]) && ch[i] != '\n');

      --i;

      bool output = true;
      if(!change_starting_line_original && diff_stack.back() == view_t::DELETE) {

        change_ending_space_original += str;
        output = false;

      }

      if(!change_starting_line_modified && diff_stack.back() == view_t::INSERT) {

        change_ending_space_modified += str;
        output = false;
      }

      if(output) {
        output_characters(str, diff_stack.back());
      }

    } else {

      assert(change_ending_space_original.empty()
             || change_ending_space_modified.empty());

      std::string str;
      do {

        str.append(1, ch[i]);
        ++i;

      } while(i < len && !isspace(ch[i]));

      --i;

      if(!change_ending_space_original.empty()) {

        output_characters(change_ending_space_original,
                          last_character_operation_original);
        change_ending_space_original = "";

      }

      if(!change_ending_space_modified.empty()) {

        output_characters(change_ending_space_modified,
                          last_character_operation_modified);
        change_ending_space_modified = "";
  
      }

      output_characters(str, diff_stack.back());

    }

  }

}

void side_by_side_view::start_unit(const std::string & local_name [[maybe_unused]],
                                   const char * prefix [[maybe_unused]],
                                   const char * URI [[maybe_unused]],
                                   int num_namespaces [[maybe_unused]],
                                   const struct srcsax_namespace * namespaces [[maybe_unused]],
                                   int num_attributes [[maybe_unused]],
                                   const struct srcsax_attribute * attributes [[maybe_unused]]) {

    add_new_line();

}

void side_by_side_view::end_unit(const std::string & local_name [[maybe_unused]], const char * prefix [[maybe_unused]], const char * URI [[maybe_unused]]) {

  if((!change_ending_space_original.empty() || !change_ending_space_modified.empty())) {

    assert(change_ending_space_original.empty() || change_ending_space_modified.empty());
    if(!change_ending_space_original.empty()) {

      output_characters(change_ending_space_original, view_t::COMMON);
      change_ending_space_original = "";

    }

    if(!change_ending_space_modified.empty()) {

      output_characters(change_ending_space_modified, view_t::COMMON);
      change_ending_space_modified = "";

    }

  }

  if(is_html) {
    output_html();
  }
  else {
    output_bash();
  }

}
