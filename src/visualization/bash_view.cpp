#include <bash_view.hpp>

#include <srcdiff_constants.hpp>

#include <shortest_edit_script.hpp>
#include <character_diff.hpp>

#include <type_query.hpp>

#include <default_theme.hpp>
#include <monokai_theme.hpp>

#include <cstring>
#include <cctype>
#include <cassert>


int bash_view::UNSET  = 0;
int bash_view::COMMON = 1 << 0;
int bash_view::DELETE = 1 << 1;
int bash_view::INSERT = 1 << 2;

const char * const bash_view::CARRIAGE_RETURN_SYMBOL = "\u23CE";

static std::string to_lower(const std::string & str) {

  std::string lower(str);
  for(char & character : lower)
    character = std::tolower(character);

  return lower;

}

bash_view::bash_view(const std::string & output_filename,
                     const std::string & syntax_highlight,
                     const std::string & theme, 
                     bool ignore_all_whitespace,
                     bool ignore_whitespace,
                     bool ignore_comments,
                     bool is_html) 
  : diff_stack(), syntax_highlight(to_lower(syntax_highlight) != "none"),
    theme(to_lower(theme) == "default" ? (theme_t *)new default_theme(to_lower(syntax_highlight), is_html) : (theme_t *)new monokai_theme(to_lower(syntax_highlight), is_html)), 
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
    save_text(false),
    saved_type(),
    saved_text()  {

  if(output_filename != "-")
    output = new std::ofstream(output_filename.c_str());
  else
    output = &std::cout;

}

bash_view::~bash_view() {

  if(output != &std::cout) {

    ((std::ofstream *)output)->close();
    delete output;

  }

  delete theme;
  
}

void bash_view::transform(const std::string & srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());

  controller.parse(this);

  reset();

}

void bash_view::reset() {

  diff_stack.clear();

  in_comment = false;
  in_literal = false;
  in_string = false;
  in_function_name = false;
  in_class_name = false;
  in_call_name = false;
  in_preprocessor_directive = false;

  close_num_span = 0;

  save_text = false,
  saved_type.clear();
  saved_text.clear();

  reset_internal();

}

std::string bash_view::change_operation_to_code(int operation) {

  if(operation == DELETE) return theme->delete_color;
  if(operation == INSERT) return theme->insert_color;

  return theme->common_color;

}

std::string bash_view::close_spans(unsigned int close_num_span) {

  if(!is_html) return std::string();

  std::string spans;
  for(unsigned int i = 0; i < close_num_span; ++i)
    spans += "</span>";

  return spans;

}

void bash_view::end_buffer(std::ostream & out, unsigned int & close_num_span) {

  out << close_spans(close_num_span);
  close_num_span = 0;

}

void bash_view::output_characters_to_buffer(std::ostream & out,
                                            const std::string & ch,
                                            int operation,
                                            int & last_character_operation,
                                            unsigned int & close_num_span) {

  if(operation != last_character_operation)
    out << close_spans(close_num_span) << change_operation_to_code(operation);

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
    if(!highlight.empty())
      out << highlight;

  }

  if(!is_html) {

    out << ch;

    if(!highlight.empty()) {
      out << theme->common_color;
      if(operation != bash_view::COMMON)
        out << change_operation_to_code(operation);
    }

    return;

  }

  close_num_span = operation == bash_view::DELETE ? 2 : 1;

  for(std::string::size_type pos = 0; pos < ch.size(); ++pos) {

    if(ch[pos] == '&')      out << "&amp;";
    else if(ch[pos] == '<') out << "&lt;";
    else if(ch[pos] == '>') out << "&gt;";
    else                    out << ch[pos];

  }

  if(!highlight.empty())
    out << "</span>";

}

void bash_view::output_character(const char c, int operation) {

  std::string ch;
  ch += c;
  output_characters(ch, operation);

}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behavior.
 */
void bash_view::startDocument() {

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
void bash_view::endDocument() {

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
void bash_view::startRoot(const char * localname,
                          const char * prefix,
                          const char * URI,
                          int num_namespaces,
                          const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes) {}

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
void bash_view::startUnit(const char * localname,
                          const char * prefix,
                          const char * URI,
                          int num_namespaces,
                          const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes) {

  diff_stack.push_back(bash_view::COMMON);

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
void bash_view::startElement(const char * localname,
                             const char * prefix,
                             const char * URI,
                             int num_namespaces,
                             const struct srcsax_namespace * namespaces,
                             int num_attributes,
                             const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(URI == SRCML_SRC_NAMESPACE_HREF && local_name == "comment") {

    in_comment = true;

  } else if(URI == SRCML_SRC_NAMESPACE_HREF && local_name == "literal") {

    std::string literal_type;
    for(int i = 0; i < num_attributes; ++i) {

      if(std::string(attributes[i].localname) == "type") {

        literal_type = attributes[i].value;
        break;

      }

    }

    if(literal_type == "string")
      in_string = true;
    else
      in_literal = true;

  } else if(URI == SRCML_SRC_NAMESPACE_HREF && local_name == "name"
            && srcml_element_stack.size() > 1) {

    const std::string & parent = srcml_element_stack.at(srcml_element_stack.size() - 2);
    if(is_function_type(parent))
      in_function_name = true;
    else if(is_class_type(parent))
      in_class_name = true;
    else if(is_call(parent))
      in_call_name = true;

  } else if(URI == SRCML_CPP_NAMESPACE_HREF
            && (local_name == "literal" || local_name == "file")) {

      in_string = true;

  } else if(URI == SRCML_CPP_NAMESPACE_HREF && local_name == "number") {

      in_literal = true;

  } else if(URI == SRCML_CPP_NAMESPACE_HREF && local_name == "directive") {

    in_preprocessor_directive = true;

  }

  start_element(local_name, prefix, URI, num_namespaces, namespaces,
                num_attributes, attributes);

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
void bash_view::endRoot(const char * localname, const char * prefix, const char * URI) {}

/**
 * endUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an unit.
 * Overide for desired behavior.
 */
void bash_view::endUnit(const char * localname, const char * prefix, const char * URI) {

  const std::string local_name(localname);
  end_unit(local_name, prefix, URI);

  diff_stack.pop_back();

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
void bash_view::endElement(const char * localname,
                           const char * prefix,
                           const char * URI) {

  const std::string local_name(localname);

  if(save_text && (local_name == "name" || local_name == "operator"))  {

    if(theme->is_keyword(saved_text.original()) || theme->is_keyword(saved_text.modified())) {

      output_characters(saved_text.original(), bash_view::DELETE);
      output_characters(saved_text.modified(), bash_view::INSERT);

    } else {

      character_diff char_diff(saved_text);
      char_diff.compute();
      char_diff.output(*this, saved_type);

    }

    save_text = false;
    saved_text.clear();

  }

  if(URI == SRCML_SRC_NAMESPACE_HREF && local_name == "comment") {

    in_comment = false;

  } else if(URI == SRCML_SRC_NAMESPACE_HREF && local_name == "literal") {

    in_literal = false;
    in_string = false;

  } else if(URI == SRCML_SRC_NAMESPACE_HREF && local_name == "name") {

    const std::string & parent = srcml_element_stack.back();
    if(is_function_type(parent))
      in_function_name = false;
    else if(is_class_type(parent))
      in_class_name = false;
    else if(is_call(parent))
      in_call_name = false;

  } else if(URI == SRCML_CPP_NAMESPACE_HREF
            && ( local_name == "literal"
              || local_name == "file"
              || local_name == "number")) {

    in_literal = false;
    in_string = false;

  } else if(URI == SRCML_CPP_NAMESPACE_HREF && local_name == "directive") {

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
void bash_view::charactersRoot(const char * ch, int len) {}

/**
 * charactersUnit
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling within a unit.
 * Overide for desired behavior.
 */
void bash_view::charactersUnit(const char * ch, int len) {

  characters(ch, len);

}

