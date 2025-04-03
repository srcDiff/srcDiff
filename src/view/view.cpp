// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file view.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <view.hpp>

#include <constants.hpp>

#include <shortest_edit_script_t.hpp>
#include <character_diff.hpp>

#include <type_query.hpp>

#include <default_theme.hpp>
#include <monokai_theme.hpp>
#include <user_defined_theme.hpp>

#include <cstring>
#include <cctype>
#include <cassert>

// unused parameters, future API usage?
int view_t::UNSET  = 0;
int view_t::COMMON = 1 << 0;
int view_t::DELETE = 1 << 1;
int view_t::INSERT = 1 << 2;

const char * const view_t::CARRIAGE_RETURN_SYMBOL = "\u23CE";

static std::string to_lower(const std::string & str) {

  std::string lower(str);
  for(char & character : lower) {
    character = std::tolower(character);
  }

  return lower;

}

view_t::view_t(const std::string & output_filename,
               const std::string & syntax_highlight,
               const std::string & theme, 
               bool ignore_all_whitespace,
               bool ignore_whitespace,
               bool ignore_comments,
               bool is_html) 
  : diff_stack(),
    srcml_stack(),
    syntax_highlight(to_lower(syntax_highlight) != "none"),
    in_comment(false),
    in_literal(false),
    in_string(false),
    in_function_name(false),
    in_class_name(false),
    in_call_name(false),
    in_preprocessor_directive(false),
    ignore_all_whitespace(ignore_all_whitespace),
    ignore_whitespace(ignore_whitespace),
    ignore_comments(ignore_comments),
    is_html(is_html),
    close_num_span(0),
    save_name(false),
    saved_name_type(),
    saved_name()  {

    const std::string theme_lower = to_lower(theme);
    const std::string syntax_level = to_lower(syntax_highlight);
    if(theme_lower == "default") {
      this->theme = (theme_t *)new default_theme(syntax_level, is_html);
    }
    else if(theme_lower == "monokai") {
      this->theme = (theme_t *)new monokai_theme(to_lower(syntax_highlight), is_html);
    }
    else {
      this->theme = (theme_t *)new user_defined_theme(to_lower(syntax_highlight), is_html, theme);
    }

  if(output_filename != "-") {
    output = new std::ofstream(output_filename.c_str());
  }
  else {
    output = &std::cout;
  }

}

view_t::~view_t() {

  if(output != &std::cout) {

    ((std::ofstream *)output)->close();
    delete output;

  }

  delete theme;
  
}

void view_t::transform(const std::string & srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());
  controller.parse(this);
  reset();

}

void view_t::transform(const char* srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());
  controller.parse(this);
  reset();

}

void view_t::reset() {

  diff_stack.clear();
  srcml_stack.clear();

  in_comment = false;
  in_literal = false;
  in_string = false;
  in_function_name = false;
  in_class_name = false;
  in_call_name = false;
  in_preprocessor_directive = false;

  close_num_span = 0;

  save_name = false,
  saved_name_type.clear();
  saved_name.clear();

  reset_internal();

}

std::string view_t::change_operation_to_code(int operation) {

  if(operation == DELETE) return theme->delete_color;
  if(operation == INSERT) return theme->insert_color;

  return theme->common_color;

}

std::string view_t::close_spans(unsigned int close_num_span) {

  if(!is_html) return std::string();

  std::string spans;
  for(unsigned int i = 0; i < close_num_span; ++i) {
    spans += "</span>";
  }

  return spans;

}

void view_t::end_buffer(std::ostream & out, unsigned int & close_num_span) {

  out << close_spans(close_num_span);
  close_num_span = 0;

}

void view_t::output_characters_to_buffer(std::ostream & out,
                                            const std::string & ch,
                                            int operation,
                                            int & last_character_operation,
                                            unsigned int & close_num_span) {

  if(ch.back() == ';' && ch.size() > 1) {
    output_characters_to_buffer(out, ch.substr(0, ch.size() - 1), operation, last_character_operation, close_num_span);
    output_characters_to_buffer(out, ";", operation, last_character_operation, close_num_span);
    return;
  } 

  if(save_name) {

    saved_name.append(ch.c_str(), ch.size(),
      diff_stack.back() == view_t::DELETE ? srcdiff::DELETE : srcdiff::INSERT);
    return;

  }

  if(operation != last_character_operation) {
    out << close_spans(close_num_span) << change_operation_to_code(operation);
  }

  last_character_operation = operation;

  if(ch.empty()) return;

  std::string highlight;
  if(syntax_highlight) {

    highlight = theme->token2color(ch,
                                   in_comment,
                                   in_literal,
                                   in_string,
                                   in_function_name,
                                   in_class_name,
                                   in_call_name,
                                   in_preprocessor_directive);
    if(!highlight.empty()) {
      out << highlight;
    }

  }

  if(!is_html) {

    out << ch;

    if(!highlight.empty()) {
      out << theme->common_color;
      if(operation != view_t::COMMON) {
        out << change_operation_to_code(operation);
      }
    }

    return;

  }

  close_num_span = operation == view_t::DELETE ? 2 : 1;

  for(std::string::size_type pos = 0; pos < ch.size(); ++pos) {

    if(ch[pos] == '&')      out << "&amp;";
    else if(ch[pos] == '<') out << "&lt;";
    else if(ch[pos] == '>') out << "&gt;";
    else                    out << ch[pos];

  }

  if(!highlight.empty()) {
    out << "</span>";
  }

}

void view_t::output_character(const char c, int operation) {

  std::string ch;
  ch += c;
  output_characters(ch, operation);

}

void view_t::srcml_stack_push(const char * localname, const char * prefix) {
  std::string full_name;
  if(prefix) {
    full_name += prefix;
    full_name += ":";
  }
  full_name += localname;
  srcml_stack.push_back(full_name);
}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behavior.
 */
void view_t::startDocument() {

  if(is_html) {

    (*output) << "<!DOCTYPE html>\n";
    (*output) << "<html>\n";
    (*output) << "<head>\n";
    (*output) << "<meta charset=\"UTF-8\">\n";
    (*output) << "<title>srcDiff</title>\n";
    (*output) << "</head>\n";
    (*output) << "<body style=\"font-family: courier, monospace; background-color: " + theme->background_color + "; color: " + theme->text_color + ";\">\n";

  } else {

    (*output) << theme->common_color;

  }

}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behavior.
 */
void view_t::endDocument() {

  if(is_html) {

    (*output) << "</body>\n";
    (*output) << "</html>\n";

  } else {

    (*output) << "\x1b[0m";

  }

}

/**
 * startRoot
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 * @param num_namespaces number of namespaces definitions
 * @param namespaces the defined namespaces
 * @param num_attributes the number of attributes on the tag
 * @param attributes list of attributes
 *
 * SAX handler function for start of the root profile.
 * Overide for desired behavior.
 */
void view_t::startRoot(const char * localname,
                          const char * prefix,
                          const char * URI [[maybe_unused]],
                          int num_namespaces [[maybe_unused]],
                          const struct srcsax_namespace * namespaces [[maybe_unused]],
                          int num_attributes [[maybe_unused]],
                          const struct srcsax_attribute * attributes [[maybe_unused]]) {

  if(is_archive) {
    diff_stack.push_back(view_t::COMMON);
    srcml_stack_push(localname, prefix);
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
void view_t::startUnit(const char * localname,
                          const char * prefix,
                          const char * URI,
                          int num_namespaces,
                          const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes) {

  diff_stack.push_back(view_t::COMMON);

  std::string langauge;
  for(int i = 0; i < num_attributes; ++i) {

    if(std::string(attributes[i].localname) == "language") {

      langauge = attributes[i].value;
      break;

    }

  }
  theme->set_langauge(langauge);

  const std::string local_name(localname);
  start_unit(local_name, prefix, URI, num_namespaces, namespaces, num_attributes, attributes);
  srcml_stack_push(localname, prefix);

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
void view_t::startElement(const char * localname,
                             const char * prefix,
                             const char * URI,
                             int num_namespaces,
                             const struct srcsax_namespace * namespaces,
                             int num_attributes,
                             const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(URI == srcdiff::SRCML_SRC_NAMESPACE_HREF && local_name == "comment") {

    in_comment = true;

  } else if(URI == srcdiff::SRCML_SRC_NAMESPACE_HREF && local_name == "literal") {

    std::string literal_type;
    for(int i = 0; i < num_attributes; ++i) {

      if(std::string(attributes[i].localname) == "type") {

        literal_type = attributes[i].value;
        break;

      }

    }

    if(literal_type == "string" || literal_type == "char") {
      in_string = true;
    }
    else {
      in_literal = true;
    }

  } else if(URI == srcdiff::SRCML_SRC_NAMESPACE_HREF && local_name == "name"
            && srcml_stack.size() > 1) {

    const std::string & parent = srcml_stack.at(srcml_stack.size() - 1);
    if(is_function_type(parent))
      in_function_name = true;
    else if(is_class_type(parent))
      in_class_name = true;
    else if(is_call(parent))
      in_call_name = true;

  } else if(URI == srcdiff::SRCML_CPP_NAMESPACE_HREF
            && (local_name == "literal" || local_name == "file")) {

      in_string = true;

  } else if(URI == srcdiff::SRCML_CPP_NAMESPACE_HREF && local_name == "number") {

      in_literal = true;

  } else if(URI == srcdiff::SRCML_CPP_NAMESPACE_HREF && local_name == "directive") {

    in_preprocessor_directive = true;

  }

  start_element(local_name, prefix, URI, num_namespaces, namespaces,
                num_attributes, attributes);
  if(URI != srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF) srcml_stack_push(localname, prefix);
}

/**
 * endRoot
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of the root profile.
 * Overide for desired behavior.
 */
void view_t::endRoot(const char * localname [[maybe_unused]], const char * prefix [[maybe_unused]], const char * URI [[maybe_unused]]) {

  if(is_archive) {
    diff_stack.pop_back();
    srcml_stack.pop_back();
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
void view_t::endUnit(const char * localname, const char * prefix, const char * URI) {

  const std::string local_name(localname);
  end_unit(local_name, prefix, URI);

  diff_stack.pop_back();
  srcml_stack.pop_back();
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
void view_t::endElement(const char * localname,
                           const char * prefix,
                           const char * URI) {

  if(URI != srcdiff::SRCDIFF_DEFAULT_NAMESPACE_HREF) srcml_stack.pop_back();

  const std::string local_name(localname);

  if(save_name && (local_name == "name" || local_name == "operator"))  {

    save_name = false;

    if(theme->is_keyword(saved_name.original()) || theme->is_keyword(saved_name.modified())) {

      output_characters(saved_name.original(), view_t::DELETE);
      output_characters(saved_name.modified(), view_t::INSERT);

    } else {

      character_diff char_diff(saved_name);
      char_diff.output(*this, saved_name_type);

    }

    saved_name.clear();

  }

  if(URI == srcdiff::SRCML_SRC_NAMESPACE_HREF && local_name == "comment") {

    in_comment = false;

  } else if(URI == srcdiff::SRCML_SRC_NAMESPACE_HREF && local_name == "literal") {

    in_literal = false;
    in_string = false;

  } else if(URI == srcdiff::SRCML_SRC_NAMESPACE_HREF && local_name == "name") {

    const std::string & parent = srcml_stack.back();
    if(is_function_type(parent)) {
      in_function_name = false;
    }
    else if(is_class_type(parent)) {
      in_class_name = false;
    }
    else if(is_call(parent)) {
      in_call_name = false;
    }

  } else if(URI == srcdiff::SRCML_CPP_NAMESPACE_HREF
            && ( local_name == "literal"
              || local_name == "file"
              || local_name == "number")) {

    in_literal = false;
    in_string = false;

  } else if(URI == srcdiff::SRCML_CPP_NAMESPACE_HREF && local_name == "directive") {

    in_preprocessor_directive = false;

  }

  end_element(local_name, prefix, URI);

}

/**
 * charactersRoot
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling at the root level.
 * Overide for desired behavior.
 */
void view_t::charactersRoot(const char * ch [[maybe_unused]], int len [[maybe_unused]]) {}

/**
 * charactersUnit
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling within a unit.
 * Overide for desired behavior.
 */
void view_t::charactersUnit(const char * ch, int len) {

  // Need diff on stack for this but not others so use element_stack here only
  if(!save_name && element_stack.size() > 1 && element_stack.back() == "diff:delete" 
    && (element_stack.at(element_stack.size() - 2) == "name"
      || element_stack.at(element_stack.size() - 2) == "operator")) {

    save_name = true;
    saved_name_type = element_stack.at(element_stack.size() - 2);

  }

  characters(ch, len);

}

