#include <unified_view.hpp>

#include <srcdiff_constants.hpp>

#include <type_query.hpp>

#include <cstring>
#include <cassert>

unified_view::unified_view(const std::string & output_filename,
                           bool syntax_highlight,
                           bool ignore_all_whitespace,
                           bool ignore_whitespace,
                           bool ignore_comments,
                           bool is_html,
                           boost::any context_type)
              : bash_view(output_filename, syntax_highlight, 
                          ignore_all_whitespace,
                          ignore_whitespace,
                          ignore_comments,
                          is_html),
                last_character_operation(bash_view::COMMON), modes(LINE), line_number_delete(0), line_number_insert(0), number_context_lines(3),
                is_after_change(false), wait_change(true), in_function(),
                context_type(context_type), length(0), is_after_additional(false),
                after_edit_count(0), last_context_line((unsigned)-1),
                change_starting_line(false), change_ending_space(),
                change_ending_operation(bash_view::COMMON),
                close_num_spans(0) {

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

}

unified_view::~unified_view() {}

void unified_view::reset_internal() {

  line_number_delete = 0;
  line_number_insert = 0;
  is_after_change = false;
  wait_change = true;
  in_function.clear();
  length = 0;
  additional_context.clear();
  is_after_additional = false;
  after_edit_count = 0;
  last_context_line = -1;
  last_character_operation = bash_view::COMMON;
  change_starting_line = false;
  change_ending_space = "";
  change_ending_operation = bash_view::COMMON;
  close_num_spans = 0;

}

unified_view::context_mode unified_view::context_string_to_id(const std::string & context_type_str) const {

  if(context_type_str == "all")      return ALL;
  if(context_type_str == "function") return FUNCTION;
  else                               return LINE;


}

bool unified_view::in_mode(context_mode mode) {

  return mode & modes;

}

void unified_view::output_additional_context() {

  size_t line_delete = line_number_delete + 1 - additional_context.size();
  size_t line_insert = line_number_insert + 1 - additional_context.size();

  if(wait_change && last_context_line != (line_number_delete - 1))
    (*output) << bash_view::COMMON_CODE << LINE_CODE << "@@ -" << line_delete << " +" << line_insert << " @@" << bash_view::COMMON_CODE << '\n';

  if(additional_context.empty()) return;

  for(std::list<std::string>::const_iterator citr = additional_context.begin(); citr != additional_context.end(); ++citr) {

    output_characters(*citr, bash_view::COMMON);

    ++line_delete, ++line_insert;

  }

  additional_context.clear();
  length = 0;

}

void unified_view::output_characters(const std::string & ch, int operation) {

  output_characters_to_buffer(*output, ch, operation, last_character_operation, close_num_spans);

}

void unified_view::start_unit(const std::string & local_name, 
                              const char * prefix, const char * URI,
                                 int num_namespaces,
                                 const struct srcsax_namespace * namespaces,
                                 int num_attributes,
                                 const struct srcsax_attribute * attributes) {}

void unified_view::start_element(const std::string & local_name, 
                                 const char * prefix, const char * URI,
                                 int num_namespaces,
                                 const struct srcsax_namespace * namespaces,
                                 int num_attributes,
                                 const struct srcsax_attribute * attributes) {


  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(ignore_comments && in_comment) return;

    if(local_name == "common")
     diff_stack.push_back(bash_view::COMMON);
    else if(local_name == "delete")
     diff_stack.push_back(DELETE);
    else if(local_name == "insert")
     diff_stack.push_back(INSERT);
    else if(local_name == "ws" && ignore_all_whitespace)
      diff_stack.push_back(bash_view::COMMON);
    
  } else {

    if(local_name == "comment") {

      if(ignore_comments)
        diff_stack.push_back(bash_view::COMMON);

    }

    if(in_mode(FUNCTION) && is_function_type(local_name)) {

      in_function.push_back(true);

      if(in_function.size() == 1) {

        additional_context.clear();
        length = 0;

      }

    }

  }

}

void unified_view::end_unit(const std::string & local_name, const char * prefix,
                               const char * URI) {}

void unified_view::end_element(const std::string & local_name, const char * prefix,
                               const char * URI) {

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(ignore_comments && in_comment) return;

      if(local_name == "common" || local_name == "delete" || local_name == "insert"
        || (local_name == "ws" && ignore_all_whitespace))
        diff_stack.pop_back();

  } else {

    if(local_name == "comment") {

      if(ignore_comments)
        diff_stack.pop_back();
      
    }

    if(in_mode(FUNCTION) && is_function_type(local_name)) {

      in_function.pop_back();
      if(in_function.size() == 0) {

        additional_context.clear();
        length = 0;

      }

    }

  }

}

void unified_view::characters(const char * ch, int len) {

  if(diff_stack.back() != bash_view::COMMON) {

   output_additional_context();

   is_after_additional = false;
   is_after_change = false;
   wait_change = false;

   output->write(context.c_str(), context.size());
   context = "";

  }

  if(!change_ending_space.empty() && change_ending_operation != diff_stack.back()) {

    output_characters(change_ending_space, bash_view::COMMON);
    change_ending_space = "";
    change_ending_operation = bash_view::COMMON;

  }

  if(last_character_operation == bash_view::COMMON && diff_stack.back() != bash_view::COMMON
     && ignore_whitespace)
    change_starting_line = true;

  for(int i = 0; i < len; ++i) {

    bool skip = false;
    if(isspace(ch[i])) {

      if(ignore_whitespace && diff_stack.back() != bash_view::COMMON) {

        if(change_starting_line) {

          output_character(ch[i], bash_view::COMMON);

        } else {

          change_ending_space += ch[i];
          change_ending_operation = diff_stack.back();

        }

        skip = true;

      }

    } else {

      if(!change_ending_space.empty()) {

        output_characters(change_ending_space, diff_stack.back());
        change_ending_space = "";
        change_ending_operation = bash_view::COMMON;

      }

      change_starting_line = false;

    }

    if(wait_change) {

      assert(!skip);
      context.append(&ch[i], 1);

    } else if(!skip) {

      if(diff_stack.back() != bash_view::COMMON && ch[i] == '\n') {

        output_characters(CARRIAGE_RETURN_SYMBOL, diff_stack.back());
        output_character(ch[i], bash_view::COMMON);

      } else {

        output_character(ch[i], diff_stack.back());

      }

    }

    if(ch[i] == '\n') {

      if(!change_ending_space.empty()) {

        output_characters(change_ending_space, bash_view::COMMON);
        change_ending_space = "";
        change_ending_operation = bash_view::COMMON;

      }

      if(diff_stack.back() != bash_view::COMMON)
        change_starting_line = true;

      if(is_after_change) {

        is_after_change = false;
        is_after_additional = true;

      } else if(is_after_additional) {

        ++after_edit_count;

        if((in_mode(LINE) && (!in_mode(FUNCTION) || !in_function.size()) && after_edit_count == number_context_lines) || (in_mode(FUNCTION) && !in_function.size())) {

          is_after_additional = false;
          after_edit_count = 0;
          wait_change = true;
          last_context_line = line_number_delete;

        }

      } else if(wait_change && ((in_mode(LINE) && (!in_mode(FUNCTION) || !in_function.size()) && number_context_lines != 0) || (in_mode(FUNCTION) && in_function.size()))) {

        if(in_mode(LINE) && (!in_mode(FUNCTION) || !in_function.size()) && length >= number_context_lines)
          additional_context.pop_front(), --length;

        additional_context.push_back(context);
        ++length;

      }

      if(diff_stack.back() != INSERT) ++line_number_delete;
      if(diff_stack.back() != DELETE) ++line_number_insert;

      context = "";

    }

  }

  if(diff_stack.back() != bash_view::COMMON) is_after_change  = true;

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
void unified_view::startUnit(const char * localname, const char * prefix,
                             const char * URI, int num_namespaces,
                             const struct srcsax_namespace * namespaces,
                             int num_attributes,
                             const struct srcsax_attribute * attributes) {

    diff_stack.push_back(bash_view::COMMON);
    output_characters("", bash_view::COMMON);

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
void unified_view::endUnit(const char * localname, const char * prefix, const char * URI) {

  if(!change_ending_space.empty()) {

    output_characters(change_ending_space, bash_view::COMMON);
    change_ending_space = "";
    change_ending_operation = bash_view::COMMON;

  }

  output_characters("", bash_view::COMMON);

}
