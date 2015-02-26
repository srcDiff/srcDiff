#ifndef INCLUDED_BASH_VIEW_HPP
#define INCLUDED_BASH_VIEW_HPP

#include <srcSAXHandler.hpp>

#include <boost/any.hpp>

#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <iostream>

class bash_view : public srcSAXHandler {

private:

  typedef unsigned long context_mode;

  static const context_mode LINE     = 1 << 0;
  static const context_mode FUNCTION = 1 << 1;
  static const context_mode ALL      = 2 << 1;

  std::vector<int> diff_stack;

  std::ostream * output;

  context_mode modes;

  size_t line_number_delete;
  size_t line_number_insert;

  std::string context;

  size_t number_context_lines;

  bool is_after_change;
  bool wait_change;
  bool in_function;

  boost::any context_type;
  std::list<std::string>::size_type length;
  std::list<std::string> additional_context;

  bool is_after_additional;
  size_t after_edit_count;
  size_t last_context_line;

public:

  bash_view(const std::string & output_filename, boost::any context_type) : modes(LINE), line_number_delete(0), line_number_insert(0), number_context_lines(3),
            is_after_change(false), wait_change(true), in_function(false), context_type(context_type), length(0),
            is_after_additional(false), after_edit_count(0), last_context_line((unsigned)-1) {

    if(context_type.type() == typeid(size_t)) {

      number_context_lines = boost::any_cast<size_t>(context_type);

    } else {

      const std::string & context_type_str = boost::any_cast<std::string>(context_type);
      const std::string::size_type dash_pos = context_type_str.find('-');
      context_mode mode = context_string_to_id(context_type_str.substr(0, dash_pos));

      if(mode == ALL) number_context_lines = -1;
      else modes |= mode;

      // assume dash is -only /** @todo actually complete this */
      if(dash_pos != std::string::npos) modes = FUNCTION;

    }

    if(output_filename != "-")
      output = new std::ofstream(output_filename.c_str());
    else
      output = &std::cout;

  }

  ~bash_view() {

    if(output != &std::cout) {

      ((std::ofstream *)output)->close();
      delete output;

    }
    
  }

  void reset() {

    line_number_delete = 0;
    line_number_insert = 0;
    is_after_change = false;
    wait_change = true;
    in_function = false;
    length = 0;
    additional_context.clear();
    is_after_additional = false;
    after_edit_count = 0;
    last_context_line = -1;


  }

private:

  bool in_mode(context_mode mode);

  void output_additional_context();

  void characters(const char * ch, int len);

  context_mode context_string_to_id(const std::string & context_type_str) const;

public:

  void transform(const std::string & srcdiff, const std::string & xml_encoding);


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
