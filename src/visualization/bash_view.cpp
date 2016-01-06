#include <bash_view.hpp>

#include <srcdiff_constants.hpp>

#include <shortest_edit_script.hpp>
#include <type_query.hpp>

#include <cstring>
#include <cassert>

const char * const bash_view::DELETE_CODE = "\x1b[9;48;5;210;1m";
const char * const bash_view::INSERT_CODE = "\x1b[48;5;120;1m";

const char * const bash_view::COMMON_CODE = "\x1b[0m";

const char * const bash_view::LINE_CODE = "\x1b[36m";

const char * const bash_view::CARRIAGE_RETURN_SYMBOL = "\u23CE";

bash_view::bash_view(const std::string & output_filename) 
  : diff_stack() {

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

const char * bash_view::change_operation_to_code(int operation) {

  if(operation == SES_DELETE) return DELETE_CODE;
  if(operation == SES_INSERT) return INSERT_CODE;

  return COMMON_CODE;

}

void bash_view::output_characters(const char c, int operation) {

  std::string ch;
  ch += c;
  output_characters(ch, operation);

}

void bash_view::output_characters(const std::string ch, int operation) {

  if(operation != last_character_operation)
    (*output) << change_operation_to_code(operation);

  last_character_operation = operation;
  (*output) << ch;

}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behavior.
 */
void bash_view::startDocument() {}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behavior.
 */
void bash_view::endDocument() {}

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
                          const struct srcsax_attribute * attributes) {}

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
                             const struct srcsax_attribute * attributes) {}

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
void bash_view::endUnit(const char * localname, const char * prefix, const char * URI) {}

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
                           const char * URI) {}

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

