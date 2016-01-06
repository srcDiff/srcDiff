#ifndef INCLUDED_SIDE_BY_SIDE_VIEW_HPP
#define INCLUDED_SIDE_BY_SIDE_VIEW_HPP

#include <bash_view.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class side_by_side_view : public bash_view {

private:

  std::vector<int> line_operations;

  int last_character_operation_original;
  std::vector<std::pair<std::ostringstream, int>> original_lines;
  int last_character_operation_modified;
  std::vector<std::pair<std::ostringstream, int>> modified_lines;

public:

  side_by_side_view(const std::string & output_filename);
  virtual ~side_by_side_view();

  void reset();

private:

  void characters(const char * ch, int len);

  void output_characters(const std::string ch, int operation);
  void add_new_line();

public:

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

};

#endif
