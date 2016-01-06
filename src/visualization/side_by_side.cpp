#include <side_by_side.hpp>

#include <srcdiff_constants.hpp>

#include <shortest_edit_script.hpp>
#include <type_query.hpp>

#include <cstring>
#include <cassert>

static const char * const DELETE_CODE = "\x1b[9;48;5;210;1m";
static const char * const INSERT_CODE = "\x1b[48;5;120;1m";

static const char * const COMMON_CODE = "\x1b[0m";

static const char * CARRIAGE_RETURN_SYMBOL = "\u23CE";

side_by_side::side_by_side(const std::string & output_filename)
  : diff_stack(), last_character_operation_original(SES_COMMON), original_lines(),
  last_character_operation_modified(SES_COMMON), modified_lines() {

  if(output_filename != "-")
    output = new std::ofstream(output_filename.c_str());
  else
    output = &std::cout;

}

side_by_side::~side_by_side() {

  if(output != &std::cout) {

    ((std::ofstream *)output)->close();
    delete output;

  }
  
}

void side_by_side::reset() {

  last_character_operation_original = SES_COMMON;
  original_lines.clear();
  last_character_operation_modified = SES_COMMON;
  modified_lines.clear();

}

void side_by_side::transform(const std::string & srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());

  controller.parse(this);

  reset();

}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behaviour.
 */
void side_by_side::startDocument() {}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behaviour.
 */
void side_by_side::endDocument() {}

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
 * Overide for desired behaviour.
 */
void side_by_side::startRoot(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
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
 * Overide for desired behaviour.
 */
void side_by_side::startUnit(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    diff_stack.push_back(SES_COMMON);
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
 * Overide for desired behaviour.
 */
void side_by_side::startElement(const char * localname, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common")
     diff_stack.push_back(SES_COMMON);
    else if(local_name == "delete")
     diff_stack.push_back(SES_DELETE);
    else if(local_name == "insert")
     diff_stack.push_back(SES_INSERT);
    
  }

}

/**
 * endRoot
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of the root profile.
 * Overide for desired behaviour.
 */
void side_by_side::endRoot(const char * localname, const char * prefix, const char * URI) {}

/**
 * endUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an unit.
 * Overide for desired behaviour.
 */
void side_by_side::endUnit(const char * localname, const char * prefix, const char * URI) {}

/**
 * endElement
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an profile.
 * Overide for desired behaviour.
 */
void side_by_side::endElement(const char * localname, const char * prefix, const char * URI) {

    const std::string local_name(localname);

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(local_name == "common" || local_name == "delete" || local_name == "insert")
        diff_stack.pop_back();

  }

}

/**
 * charactersRoot
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling at the root level.
 * Overide for desired behaviour.
 */
void side_by_side::charactersRoot(const char * ch, int len) {}

static const char * change_operation_to_code(int operation) {

  if(operation == SES_DELETE) return DELETE_CODE;
  if(operation == SES_INSERT) return INSERT_CODE;

  return COMMON_CODE;

}

void side_by_side::output_characters(const char c, int operation) {

  std::string ch;
  ch += c;
  output_characters(ch, operation);

}

void output_characters_to_buffer(const std::string ch, int operation,
                                 std::string & buffer,
                                 int & last_character_operation) {

  if(operation != last_character_operation)
    buffer += change_operation_to_code(operation);

  last_character_operation = operation;
  buffer += ch;  

}

void side_by_side::output_characters(const std::string ch, int operation) {

  if(operation != SES_INSERT)
    output_characters_to_buffer(ch, operation, original_lines.back(),
                                last_character_operation_original);

  if(operation != SES_DELETE)
    output_characters_to_buffer(ch, operation, modified_lines.back(),
                                last_character_operation_modified);



}

void side_by_side::add_new_line() {

  original_lines.push_back("");
  modified_lines.push_back("");

}

void side_by_side::characters(const char * ch, int len) {

  for(int i = 0; i < len; ++i) {

    if(ch[i] == '\n') {

      add_new_line();
      continue;

    }





  }

}

/**
 * charactersUnit
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling within a unit.
 * Overide for desired behaviour.
 */
void side_by_side::charactersUnit(const char * ch, int len) {

  characters(ch, len);

}

