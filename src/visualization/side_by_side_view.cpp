#include <side_by_side_view.hpp>

#include <srcdiff_constants.hpp>

#include <algorithm>

#include <cstring>
#include <cassert>

side_by_side_view::side_by_side_view(const std::string & output_filename)
  : bash_view(output_filename), line_operations(),
    last_character_operation_original(bash_view::COMMON), original_lines(),
    last_character_operation_modified(bash_view::COMMON), modified_lines() {}

side_by_side_view::~side_by_side_view() {}

void side_by_side_view::reset() {

  line_operations.clear();

  last_character_operation_original = bash_view::COMMON;
  original_lines.clear();

  last_character_operation_modified = bash_view::COMMON;
  modified_lines.clear();

}

void side_by_side_view::output_characters(const std::string ch, int operation) {

  if(original_lines.empty()) add_new_line();

  if(operation != bash_view::INSERT) {

    output_characters_to_buffer(ch, operation, original_lines.back().first,
                                last_character_operation_original);
    original_lines.back().second += 1;

  }

  if(operation != bash_view::DELETE) {

    output_characters_to_buffer(ch, operation, modified_lines.back().first,
                                last_character_operation_modified);
    modified_lines.back().second += 1;

  }

  line_operations.back() |= operation;

}

void side_by_side_view::add_new_line() {

  last_character_operation_original = COMMON;
  original_lines.emplace_back(std::ostringstream(), 0);

  last_character_operation_modified = COMMON;
  modified_lines.emplace_back(std::ostringstream(), 0);

  line_operations.push_back(0);

}

void side_by_side_view::characters(const char * ch, int len) {

  for(int i = 0; i < len; ++i) {

    if(ch[i] == '\n') {

      if(diff_stack.back() != COMMON)
        output_characters(CARRIAGE_RETURN_SYMBOL, diff_stack.back());
      else
        line_operations.back() |= COMMON;

      add_new_line();
      continue;

    }

    output_character(ch[i], diff_stack.back());

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
void side_by_side_view::startUnit(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    diff_stack.push_back(bash_view::COMMON);
    add_new_line();

}

/**
 * startElement
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 * @param num_namespaces number of namespaces definitions
 * @param namespaces the defined namespaces
 * @param num_attributes the number of attributes on the tag
 * @param attributes list of attributes
 *
 * SAX handler function for start of an profile.
 * Overide for desired behavior.
 */
void side_by_side_view::startElement(const char * localname, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common")
     diff_stack.push_back(bash_view::COMMON);
    else if(local_name == "delete")
     diff_stack.push_back(bash_view::DELETE);
    else if(local_name == "insert")
     diff_stack.push_back(bash_view::INSERT);
    
  }

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
void side_by_side_view::endUnit(const char * localname, const char * prefix, const char * URI) {

  /** @todo handle tabs */
  int max_width = 0;
  for(const std::pair<std::ostringstream, int> & line : original_lines)
    max_width = std::max(max_width, line.second);

  for(int i = 0; i < original_lines.size(); ++i) {

    (*output) << bash_view::COMMON_CODE << original_lines[i].first.str();

    std::string fill(max_width - original_lines[i].second, ' ');
    (*output) << bash_view::COMMON_CODE << fill;

    if(line_operations[i] == bash_view::DELETE)
      (*output) << " < ";
    else if(line_operations[i] == bash_view::INSERT)
      (*output) << " > ";
    else
      (*output) << " | ";

    (*output) << modified_lines[i].first.str();

    (*output) << bash_view::COMMON_CODE << '\n';

  }

}

/**
 * endElement
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an profile.
 * Overide for desired behavior.
 */
void side_by_side_view::endElement(const char * localname, const char * prefix, const char * URI) {

    const std::string local_name(localname);

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(local_name == "common" || local_name == "delete" || local_name == "insert")
        diff_stack.pop_back();

  }

}
