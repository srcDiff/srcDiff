#include <bash_view.hpp>

#include <srcdiff_constants.hpp>

#include <shortest_edit_script.hpp>
#include <type_query.hpp>

#include <cstring>
#include <cassert>


int bash_view::UNSET  = 0;
int bash_view::COMMON = 1 << 0;
int bash_view::DELETE = 1 << 1;
int bash_view::INSERT = 1 << 2;

const char * const bash_view::DELETE_CODE = "\x1b[9;48;5;210;1m";
const char * const bash_view::INSERT_CODE = "\x1b[48;5;120;1m";

const char * const bash_view::COMMON_CODE = "\x1b[0m";

const char * const bash_view::LINE_CODE = "\x1b[36m";

const char * const bash_view::DELETE_CODE_HTML = "<span style=\"color:grey; text-decoration: line-through;\"><span style=\"color: black; background-color: rgb(255,187,187); font-weight: bold;\">";
const char * const bash_view::INSERT_CODE_HTML = "<span style=\"background-color: rgb(0,250,108)  ; font-weight: bold;\">";
const char * const bash_view::COMMON_CODE_HTML = "<span style=\"background-color: transparent\">";

const char * const bash_view::CARRIAGE_RETURN_SYMBOL = "\u23CE";

bash_view::bash_view(const std::string & output_filename,
                     bool syntax_highlight,
                     bool ignore_all_whitespace,
                     bool ignore_whitespace,
                     bool ignore_comments,
                     bool is_html) 
  : diff_stack(), syntax_highlight(syntax_highlight), highlighter(), 
    ignore_all_whitespace(ignore_all_whitespace),
    ignore_whitespace(ignore_whitespace), ignore_comments(ignore_comments),
    in_comment(false), is_html(is_html), close_num_span(0) {

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
  
}

void bash_view::transform(const std::string & srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());

  controller.parse(this);

  reset();

}

void bash_view::reset() {

  diff_stack.clear();
  in_comment = false;
  close_num_span = 0;

  reset_internal();

}

const char * bash_view::change_operation_to_code(int operation) {

  if(!is_html) {

    if(operation == DELETE) return DELETE_CODE;
    if(operation == INSERT) return INSERT_CODE;

    return COMMON_CODE;

  }

  if(operation == DELETE) return DELETE_CODE_HTML;
  if(operation == INSERT) return INSERT_CODE_HTML;

    return COMMON_CODE_HTML;

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

  std::string highlight;
  if(syntax_highlight) {

    highlight = highlighter.token2color(ch);
    if(!highlight.empty())
      out << highlight;

  }

  if(!is_html) {

    out << ch;
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

  if(is_html) (*output) << "<!DOCTYPE html>\n<html><head><meta charset=\"UTF-8\"><title>srcDiff</title></head><body>";

}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behavior.
 */
void bash_view::endDocument() {

  if(is_html) (*output) << "</body></html>";

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
void bash_view::startRoot(const char * localname, const char * prefix, const char * URI,
                          int num_namespaces, const struct srcsax_namespace * namespaces,
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
void bash_view::startUnit(const char * localname, const char * prefix, const char * URI,
                          int num_namespaces, const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes) {

  diff_stack.push_back(bash_view::COMMON);

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
void bash_view::startElement(const char * localname, const char * prefix,
                             const char * URI, int num_namespaces,
                             const struct srcsax_namespace * namespaces,
                             int num_attributes,
                             const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(URI != SRCDIFF_DEFAULT_NAMESPACE_HREF && local_name == "comment")
    in_comment = true;

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
void bash_view::endElement(const char * localname, const char * prefix,
                           const char * URI) {

  const std::string local_name(localname);
  if(URI != SRCDIFF_DEFAULT_NAMESPACE_HREF && local_name == "comment")
    in_comment = false;

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

