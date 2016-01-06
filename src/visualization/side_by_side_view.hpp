#ifndef INCLUDED_SIDE_BY_SIDE_VIEW_HPP
#define INCLUDED_SIDE_BY_SIDE_VIEW_HPP

#include <srcSAXHandler.hpp>

#include <boost/any.hpp>

#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <iostream>

class side_by_side_view : public srcSAXHandler {

private:

  std::vector<int> diff_stack;

  std::ostream * output;

  int last_character_operation_original;
  std::vector<std::pair<std::string, int>> original_lines;
  int last_character_operation_modified;
  std::vector<std::pair<std::string, int>> modified_lines;

public:

  side_by_side_view(const std::string & output_filename);
  ~side_by_side_view();

  void transform(const std::string & srcdiff, const std::string & xml_encoding);
  void reset();

private:

  void characters(const char * ch, int len);

  void output_characters(const char c, int operation);
  void output_characters(const std::string ch, int operation);

  void add_new_line();

public:

  /**
   * startDocument
   *
   * SAX handler function for start of document.
   * Overide for desired behaviour.
   */
  virtual void startDocument();

  /**
   * endDocument
   *
   * SAX handler function for end of document.
   * Overide for desired behaviour.
   */
  virtual void endDocument();

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
  virtual void startRoot(const char * localname, const char * prefix, const char * URI,
                         int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                         const struct srcsax_attribute * attributes);

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
  virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                         int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                         const struct srcsax_attribute * attributes);

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
  virtual void startElement(const char * localname, const char * prefix, const char * URI,
                              int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                              const struct srcsax_attribute * attributes);

  /**
   * endRoot
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of the root profile.
   * Overide for desired behaviour.
   */
  virtual void endRoot(const char * localname, const char * prefix, const char * URI);

  /**
   * endUnit
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of an unit.
   * Overide for desired behaviour.
   */
  virtual void endUnit(const char * localname, const char * prefix, const char * URI);

  /**
   * endElement
   * @param localname the name of the profile tag
   * @param prefix the tag prefix
   * @param URI the namespace of tag
   *
   * SAX handler function for end of an profile.
   * Overide for desired behaviour.
   */
  virtual void endElement(const char * localname, const char * prefix, const char * URI);

  /**
   * charactersRoot
   * @param ch the characers
   * @param len number of characters
   *
   * SAX handler function for character handling at the root level.
   * Overide for desired behaviour.
   */
  virtual void charactersRoot(const char * ch, int len);

  /**
   * charactersUnit
   * @param ch the characers
   * @param len number of characters
   *
   * SAX handler function for character handling within a unit.
   * Overide for desired behaviour.
   */
  virtual void charactersUnit(const char * ch, int len);

};

#endif
