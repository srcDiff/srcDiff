#include <unified_view.hpp>

#include <srcdiff_constants.hpp>

#include <shortest_edit_script.hpp>
#include <type_query.hpp>

#include <cstring>
#include <cassert>

const char * const DELETE_CODE = "\x1b[9;48;5;210;1m";
const char * const INSERT_CODE = "\x1b[48;5;120;1m";

const char * const COMMON_CODE = "\x1b[0m";

const char * const LINE_CODE = "\x1b[36m";

const char * CARRIAGE_RETURN_SYMBOL = "\u23CE";

unified_view::unified_view(const std::string & output_filename, boost::any context_type,
               bool ignore_all_whitespace, bool ignore_whitespace, bool ignore_comments)
              : ignore_all_whitespace(ignore_all_whitespace), ignore_whitespace(ignore_whitespace),
                ignore_comments(ignore_comments), last_character_operation(SES_COMMON),
                modes(LINE), line_number_delete(0), line_number_insert(0),
                number_context_lines(3), is_after_change(false), wait_change(true),
                in_function(), context_type(context_type), length(0), 
                is_after_additional(false), after_edit_count(0),
                last_context_line((unsigned)-1), change_starting_line(false),
                change_ending_space(), change_ending_operation(SES_COMMON),
                in_comment(false) {

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

unified_view::~unified_view() {

  if(output != &std::cout) {

    ((std::ofstream *)output)->close();
    delete output;

  }
  
}

void unified_view::reset() {

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
  last_character_operation = SES_COMMON;
  change_starting_line = false;
  change_ending_space = "";
  change_ending_operation = SES_COMMON;
  in_comment = false;


}

void unified_view::transform(const std::string & srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());

  controller.parse(this);

  reset();

}

unified_view::context_mode unified_view::context_string_to_id(const std::string & context_type_str) const {

  if(context_type_str == "all")      return ALL;
  if(context_type_str == "function") return FUNCTION;
  else                               return LINE;


}

bool unified_view::in_mode(context_mode mode) {

  return mode & modes;

}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behaviour.
 */
void unified_view::startDocument() {}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behaviour.
 */
void unified_view::endDocument() {}

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
void unified_view::startRoot(const char * localname, const char * prefix, const char * URI,
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
void unified_view::startUnit(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    diff_stack.push_back(SES_COMMON);
    output_characters("", SES_COMMON);

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
void unified_view::startElement(const char * localname, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(ignore_comments && in_comment) return;

    if(local_name == "common")
     diff_stack.push_back(SES_COMMON);
    else if(local_name == "delete")
     diff_stack.push_back(SES_DELETE);
    else if(local_name == "insert")
     diff_stack.push_back(SES_INSERT);
    else if(local_name == "ws" && ignore_all_whitespace)
      diff_stack.push_back(SES_COMMON);
    
  } else {

    if(local_name == "comment") {

      in_comment = true;
      if(ignore_comments)
        diff_stack.push_back(SES_COMMON);

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

/**
 * endRoot
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of the root profile.
 * Overide for desired behaviour.
 */
void unified_view::endRoot(const char * localname, const char * prefix, const char * URI) {}

/**
 * endUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an unit.
 * Overide for desired behaviour.
 */
void unified_view::endUnit(const char * localname, const char * prefix, const char * URI) {}

/**
 * endElement
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an profile.
 * Overide for desired behaviour.
 */
void unified_view::endElement(const char * localname, const char * prefix, const char * URI) {

    const std::string local_name(localname);

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(ignore_comments && in_comment) return;

      if(local_name == "common" || local_name == "delete" || local_name == "insert"
        || (local_name == "ws" && ignore_all_whitespace))
        diff_stack.pop_back();

  } else {

    if(local_name == "comment") {

      in_comment = false;
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

/**
 * charactersRoot
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling at the root level.
 * Overide for desired behaviour.
 */
void unified_view::charactersRoot(const char * ch, int len) {}

void unified_view::output_additional_context() {

  size_t line_delete = line_number_delete + 1 - additional_context.size();
  size_t line_insert = line_number_insert + 1 - additional_context.size();

  if(wait_change && last_context_line != (line_number_delete - 1))
    (*output) << COMMON_CODE << LINE_CODE << "@@ -" << line_delete << " +" << line_insert << " @@" << COMMON_CODE << '\n';

  if(additional_context.empty()) return;

  for(std::list<std::string>::const_iterator citr = additional_context.begin(); citr != additional_context.end(); ++citr) {

    output_characters(*citr, SES_COMMON);

    ++line_delete, ++line_insert;

  }

  additional_context.clear();
  length = 0;

}

const char * change_operation_to_code(int operation) {

  if(operation == SES_DELETE) return DELETE_CODE;
  if(operation == SES_INSERT) return INSERT_CODE;

  return COMMON_CODE;

}

void unified_view::output_characters(const char c, int operation) {

  std::string ch;
  ch += c;
  output_characters(ch, operation);

}

void unified_view::output_characters(const std::string ch, int operation) {

  if(operation != last_character_operation)
    (*output) << change_operation_to_code(operation);

  last_character_operation = operation;
  (*output) << ch;

}

void unified_view::characters(const char * ch, int len) {

  if(!change_ending_space.empty() && change_ending_operation != diff_stack.back()) {

    output_characters(change_ending_space, diff_stack.back());
    change_ending_space = "";
    change_ending_operation = SES_COMMON;

  }

  const char * code = change_operation_to_code(diff_stack.back());

  for(int i = 0; i < len; ++i) {

    bool skip = false;
    if(isspace(ch[i])) {

      if(ignore_whitespace && diff_stack.back() != SES_COMMON) {

        if(change_starting_line) {

          output_characters(ch[i], SES_COMMON);

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
        change_ending_operation = SES_COMMON;

      }

      change_starting_line = false;

    }

    if(wait_change) {

      assert(!skip);
      context.append(&ch[i], 1);

    } else if(!skip) {

      if(diff_stack.back() != SES_COMMON && ch[i] == '\n') {

        output_characters(CARRIAGE_RETURN_SYMBOL, diff_stack.back());
        output_characters(ch[i], SES_COMMON);

      } else {

        output_characters(ch[i], diff_stack.back());

      }

    }

    if(ch[i] == '\n') {

      if(!change_ending_space.empty()) {

        output_characters(change_ending_space, SES_COMMON);
        change_ending_space = "";
        change_ending_operation = SES_COMMON;

      }

      if(diff_stack.back() != SES_COMMON)
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

      if(code == COMMON_CODE || code == DELETE_CODE) ++line_number_delete;
      if(code == COMMON_CODE || code == INSERT_CODE) ++line_number_insert;

      context = "";

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
void unified_view::charactersUnit(const char * ch, int len) {

  if(diff_stack.back() != SES_COMMON) {

   output_additional_context();

   is_after_additional = false;
   is_after_change = false;
   wait_change = false;

   output->write(context.c_str(), context.size());
   context = "";

  }

  characters(ch, len);

  if(diff_stack.back() != SES_COMMON) is_after_change  = true;

}

