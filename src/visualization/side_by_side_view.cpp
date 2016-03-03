#include <side_by_side_view.hpp>

#include <srcdiff_constants.hpp>

#include <algorithm>

#include <cstring>
#include <cassert>
#include <iomanip>

side_by_side_view::side_by_side_view(const std::string & output_filename,
                                     bool ignore_all_whitespace, bool ignore_whitespace,
                                     bool ignore_comments, bool is_html,
                                     int side_by_side_tab_size)
  : bash_view(output_filename, ignore_all_whitespace, ignore_whitespace,
              ignore_comments, is_html),
    side_by_side_tab_size(side_by_side_tab_size), line_operations(),
    last_character_operation_original(bash_view::COMMON), original_lines(),
    last_character_operation_modified(bash_view::COMMON), modified_lines() {}

side_by_side_view::~side_by_side_view() {}

void side_by_side_view::reset_internal() {

  line_operations.clear();

  last_character_operation_original = bash_view::COMMON;
  original_lines.clear();

  last_character_operation_modified = bash_view::COMMON;
  modified_lines.clear();

  change_starting_line_original = true;
  change_starting_line_modified = true;

  change_ending_space_original = "";
  change_ending_space_modified = "";

}

void side_by_side_view::output_characters(const std::string ch, int operation) {

  if(original_lines.empty()) add_new_line();

  int real_operation = operation;
  bool is_ignore = false;
  if(!ch.empty() && isspace(ch[0])) {

    if(ignore_all_whitespace)
      real_operation = bash_view::COMMON;

    if(ignore_all_whitespace || ignore_whitespace)
      is_ignore = true;

  }

  if(ignore_comments && in_comment) {

    real_operation = bash_view::COMMON;
    is_ignore = true;

  }

  int size = ch == CARRIAGE_RETURN_SYMBOL ? 1 : ch.size();

  if(operation != bash_view::INSERT) {

    if(change_starting_line_original && !ch.empty() && !isspace(ch[0]))
      change_starting_line_original = false;

    if(ignore_whitespace && change_starting_line_original
       && operation != bash_view::COMMON)
      real_operation = bash_view::COMMON;

    output_characters_to_buffer(ch, real_operation,
                                std::get<0>(original_lines.back()),
                                last_character_operation_original);
    std::get<1>(original_lines.back()) += size;

  }

  if(operation != bash_view::DELETE) {

    if(change_starting_line_modified && !ch.empty() && !isspace(ch[0]))
      change_starting_line_modified = false;

    if(ignore_whitespace && change_starting_line_modified
       && operation != bash_view::COMMON)
      real_operation = bash_view::COMMON;

    output_characters_to_buffer(ch, real_operation,
                                std::get<0>(modified_lines.back()),
                                last_character_operation_modified);
    std::get<1>(modified_lines.back()) += size;

  }

  if(!is_ignore)
    line_operations.back() |= operation;

}

void side_by_side_view::add_new_line() {

  last_character_operation_original = COMMON;
  original_lines.emplace_back(std::ostringstream(), 0, 0);

  last_character_operation_modified = COMMON;
  modified_lines.emplace_back(std::ostringstream(), 0, 0);

  line_operations.push_back(0);

  change_starting_line_original = true;
  change_starting_line_modified = true;

}

void side_by_side_view::start_element(const std::string & local_name, 
                                         const char * prefix, const char * URI,
                                         int num_namespaces,
                                         const struct srcsax_namespace * namespaces,
                                         int num_attributes,
                                         const struct srcsax_attribute * attributes) {

  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common")
     diff_stack.push_back(bash_view::COMMON);
    else if(local_name == "delete")
     diff_stack.push_back(bash_view::DELETE);
    else if(local_name == "insert")
     diff_stack.push_back(bash_view::INSERT);

    if(local_name != "ws") {

      if(diff_stack.back() != bash_view::INSERT)
        change_starting_line_original = true;
   
      if(diff_stack.back() != bash_view::DELETE)
        change_starting_line_modified = true;

    }
      
  }

}

void side_by_side_view::end_element(const std::string & local_name, const char * prefix,
                               const char * URI) {

  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common" || local_name == "delete" || local_name == "insert")
      diff_stack.pop_back();

  }

}

void side_by_side_view::characters(const char * ch, int len) {

  if((!change_ending_space_original.empty() || !change_ending_space_modified.empty())) {

    assert(change_ending_space_original.empty() || change_ending_space_modified.empty());
    if(!change_ending_space_original.empty() && diff_stack.back() != bash_view::DELETE) {

      output_characters(change_ending_space_original, bash_view::COMMON);
      change_ending_space_original = "";

    }

    if(!change_ending_space_modified.empty() && diff_stack.back() != bash_view::INSERT) {

      output_characters(change_ending_space_modified, bash_view::COMMON);
      change_ending_space_modified = "";

    }

  }

  for(int i = 0; i < len; ++i) {

    std::string str;
    if(ch[i] == '\t')
      str.append(side_by_side_tab_size, ' ');
    else
      str.append(1, ch[i]);

    if(str[0] == '\n') {

      if(!ignore_all_whitespace && !ignore_whitespace) {

        if(diff_stack.back() != bash_view::COMMON)
          output_characters(CARRIAGE_RETURN_SYMBOL, diff_stack.back());
        else
          line_operations.back() |= bash_view::COMMON;

      } else {

        line_operations.back() |= diff_stack.back();

      }


      add_new_line();
      continue;

    }

    if(isspace(str[0]) && ignore_whitespace && diff_stack.back() != bash_view::COMMON) {

      bool output = true;
      if(!change_starting_line_original && diff_stack.back() == bash_view::DELETE) {

        change_ending_space_original += str;
        output = false;

      }

      if(!change_starting_line_modified && diff_stack.back() == bash_view::INSERT) {

        change_ending_space_modified += str;
        output = false;
      }

      if(output)
        output_characters(str, diff_stack.back());

    } else {

      assert(change_ending_space_original.empty()
             || change_ending_space_modified.empty());

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

/**
 * startUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 * @param num_namespaces number of namespaces definitions
 * @param namespaces the defined namespaces
 * @param num_attributes the number of attributes on the tag
 * @param attributes list of attributes
 *
 * SAX handler function for start of an unit.
 * Overide for desired behavior.
 */
void side_by_side_view::startUnit(const char * localname, const char * prefix,
                                  const char * URI, int num_namespaces,
                                  const struct srcsax_namespace * namespaces,
                                  int num_attributes,
                                  const struct srcsax_attribute * attributes) {

    diff_stack.push_back(bash_view::COMMON);
    add_new_line();

}

/**
 * endUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an unit.
 * Overide for desired behavior.
 */
void side_by_side_view::endUnit(const char * localname, const char * prefix,
                                const char * URI) {

  if((!change_ending_space_original.empty() || !change_ending_space_modified.empty())) {

    assert(change_ending_space_original.empty() || change_ending_space_modified.empty());
    if(!change_ending_space_original.empty()) {

      output_characters(change_ending_space_original, bash_view::COMMON);
      change_ending_space_original = "";

    }

    if(!change_ending_space_modified.empty()) {

      output_characters(change_ending_space_modified, bash_view::COMMON);
      change_ending_space_modified = "";

    }

  }

  if(is_html) {

    int magnitude = 0;
    size_t lines = line_operations.size();
    while(lines > 0) {

      lines /= 10;
      ++magnitude;

    }

    (*output) << "<div style=\"font-family: courier, monospace;\">";

    (*output) << "<div style=\"float: left; border: 1px solid black; border-collapse: collapse; padding: 5px;\">";
    (*output) << "<table><tr><th><strong>Original</strong></th></tr><tr><td><pre>";

    int line_number = 1;
    for(int i = 0; i < original_lines.size(); ++i) {

      (*output) << "<span><span style=\"color: grey\">";

      if(line_operations[i] != bash_view::INSERT) {

        (*output) << std::right << std::setw(magnitude) << std::setfill(' ') << line_number << ' ';
        ++line_number;

      } else {

        (*output) << std::string(' ', magnitude + 1);

      }

      (*output) << bash_view::COMMON_CODE_HTML;
      (*output) << std::get<0>(original_lines[i]).str();
      (*output) << bash_view::COMMON_CODE_HTML << '\n' << "</span></span>";  

    }
    (*output) << "</pre></td></tr></table></div>";

    (*output) << "<div style=\"float: left; border: 1px solid black; border-collapse: collapse; padding: 5px;\">";
    (*output) << "<table><tr><th><strong>Modified</strong></th></tr><tr><td><pre>";

    line_number = 1;
    for(int i = 0; i < modified_lines.size(); ++i) {

      (*output) << "<span><span style=\"color: grey\">";

      if(line_operations[i] != bash_view::DELETE) {

        (*output) << std::right << std::setw(magnitude) << std::setfill(' ') << line_number << ' ';
        ++line_number;

      } else {

        (*output) << std::string(' ', magnitude + 1);

      }

      (*output) << bash_view::COMMON_CODE_HTML;
      (*output) << std::get<0>(modified_lines[i]).str();
      (*output) << bash_view::COMMON_CODE_HTML << '\n' << "</span></span>";  

    }
    (*output) << "</pre></td></tr></table></div>";

    (*output) << "</div>";

  } else {

    int max_width = 0;
    for(const std::tuple<std::ostringstream, int, size_t> & line : original_lines)
      max_width = std::max(max_width, std::get<1>(line));

    for(int i = 0; i < original_lines.size(); ++i) {

      (*output) << bash_view::COMMON_CODE << std::get<0>(original_lines[i]).str();

      std::string fill(max_width - std::get<1>(original_lines[i]), ' ');
      (*output) << bash_view::COMMON_CODE << fill;

      if(line_operations[i] == bash_view::DELETE)
        (*output) << " < ";
      else if(line_operations[i] == bash_view::INSERT)
        (*output) << " > ";
      else if(line_operations[i] == bash_view::COMMON)
        (*output) << "   ";
      else if(line_operations[i] == 0)
        (*output) << "   ";
      else
        (*output) << " | ";

      (*output) << std::get<0>(modified_lines[i]).str();

      (*output) << bash_view::COMMON_CODE << '\n';


    }

  }

}
