#ifndef INCLUDED_SIDE_BY_SIDE_VIEW_HPP
#define INCLUDED_SIDE_BY_SIDE_VIEW_HPP

#include <bash_view.hpp>

#include <versioned_string.hpp>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class side_by_side_view : public bash_view {

private:

  int side_by_side_tab_size;

  std::vector<int> line_operations;

  static const constexpr int STREAM      = 0;
  static const constexpr int OPERATION   = 1;
  static const constexpr int CLOSE_SPANS = 2;
  static const constexpr int LINE_INCR   = 3;

  int last_character_operation_original;
  std::vector<std::tuple<std::ostringstream, int, unsigned int, size_t>> original_lines;
  int last_character_operation_modified;
  std::vector<std::tuple<std::ostringstream, int, unsigned int, size_t>> modified_lines;

  bool change_starting_line_original;
  bool change_starting_line_modified;

  std::string change_ending_space_original;
  std::string change_ending_space_modified;

  bool save_text;
  versioned_string saved_text;

public:

  side_by_side_view(const std::string & output_filename,
                    const std::string & syntax_highlight,
                    const std::string & theme,
                    bool ignore_all_whitespace,
                    bool ignore_whitespace,
                    bool ignore_comments,
                    bool is_html,
                    int side_by_side_tab_size);
  virtual ~side_by_side_view();

private:

  void reset_internal();

  virtual void start_unit(const std::string & local_name,
                          const char * prefix,
                          const char * URI,
                          int num_namespaces,
                          const struct srcsax_namespace * namespaces,
                          int num_attributes,
                          const struct srcsax_attribute * attributes);
  virtual void end_unit(const std::string & local_name,
                        const char * prefix,
                        const char * URI);
  virtual void start_element(const std::string & local_name,
                             const char * prefix,
                             const char * URI,
                            int num_namespaces,
                            const struct srcsax_namespace * namespaces,
                            int num_attributes,
                            const struct srcsax_attribute * attributes);
  virtual void end_element(const std::string & local_name,
                           const char * prefix,
                           const char * URI);
  virtual void characters(const char * ch, int len);

  void add_new_line();

  void output_html();
  void output_bash();

public:
  void output_characters(const std::string & ch, int operation);

};

#endif
