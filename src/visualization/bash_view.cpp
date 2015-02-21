#include <bash_view.hpp>
#include <shortest_edit_script.hpp>
#include <type_query.hpp>

#include <cstring>

const char * const DELETE_CODE = "\x1b[9;48;5;210;1m";
const char * const INSERT_CODE = "\x1b[48;5;120;1m";

const char * const COMMON_CODE = "\x1b[0m";

const char * const LINE_CODE = "\x1b[36m";

const char * CARRIAGE_RETURN_SYMBOL = "\u23CE";

void bash_view::transform(const std::string & srcdiff, const std::string & xml_encoding) {

  srcSAXController controller(srcdiff, xml_encoding.c_str());

  controller.parse(this);

}

bash_view::context_mode bash_view::context_string_to_id(const std::string & context_type_str) const {

  if(context_type_str == "all")      return ALL;
  if(context_type_str == "function") return FUNCTION;
  else                               return LINE;


}

bool bash_view::in_mode(context_mode mode) {

  return mode & modes;

}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behaviour.
 */
void bash_view::startDocument() {}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behaviour.
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
 * Overide for desired behaviour.
 */
void bash_view::startRoot(const char * localname, const char * prefix, const char * URI,
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
void bash_view::startUnit(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    diff_stack.push_back(SESCOMMON);

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
void bash_view::startElement(const char * localname, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes) {

  const std::string local_name(localname);

  if(strcmp(URI, "http://www.sdml.info/srcDiff") == 0) {

    if(local_name == "common")
     diff_stack.push_back(SESCOMMON);
    else if(local_name == "delete")
     diff_stack.push_back(SESDELETE);
    else if(local_name == "insert")
     diff_stack.push_back(SESINSERT);
    
  } else {

    if(in_mode(FUNCTION) && is_function_type(local_name)) in_function = true;

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
void bash_view::endRoot(const char * localname, const char * prefix, const char * URI) {}

/**
 * endUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an unit.
 * Overide for desired behaviour.
 */
void bash_view::endUnit(const char * localname, const char * prefix, const char * URI) {}

/**
 * endElement
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an profile.
 * Overide for desired behaviour.
 */
void bash_view::endElement(const char * localname, const char * prefix, const char * URI) {

    const std::string local_name(localname);

    if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

      if(local_name == "common" || local_name == "delete" || local_name == "insert")
        diff_stack.pop_back();
  } else {

    if(in_mode(FUNCTION) && is_function_type(local_name)) {

      in_function = false;
      additional_context.clear();
      length = 0;

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
void bash_view::charactersRoot(const char * ch, int len) {}

void bash_view::output_additional_context() {


  size_t line_delete = line_number_delete + 1 - additional_context.size();
  size_t line_insert = line_number_insert + 1 - additional_context.size();

  if(wait_change && last_context_line != (line_number_delete - 1))
    (*output) << COMMON_CODE << LINE_CODE << "@@ -" << line_delete << " +" << line_insert << " @@" << COMMON_CODE;

  if(additional_context.empty()) return;

  for(std::list<std::string>::const_iterator citr = additional_context.begin(); citr != additional_context.end(); ++citr) {

    (*output) << *citr;

    ++line_delete, ++line_insert;

  }

  additional_context.clear();
  length = 0;

}

void bash_view::characters(const char * ch, int len) {

  size_t number_context_lines = -1;
  if(in_mode(LINE)) number_context_lines = boost::any_cast<size_t>(context_type);

  const char * code = COMMON_CODE;
  if(diff_stack.back() == SESDELETE) code = DELETE_CODE;
  else if(diff_stack.back() == SESINSERT) code = INSERT_CODE;

  if(code != COMMON_CODE) (*output) << code;

  for(int i = 0; i < len; ++i) {

    if(wait_change) {

      context.append(&ch[i], 1);

    } else {

      if(code != COMMON_CODE && ch[i] == '\n') (*output) << CARRIAGE_RETURN_SYMBOL << COMMON_CODE;
      (*output) << ch[i];

      if(code != COMMON_CODE && ch[i] == '\n') (*output) << code;

    }

    if(ch[i] == '\n') {

      if(is_after_change) {

        is_after_change = false;
        is_after_additional = true;

      } else if(is_after_additional) {

        ++after_edit_count;

        if((in_mode(LINE) && (!in_mode(FUNCTION) || !in_function) && after_edit_count == number_context_lines) || (in_mode(FUNCTION) && !in_function)) {

          is_after_additional = false;
          after_edit_count = 0;
          wait_change = true;
          last_context_line = line_number_delete;

        }

      } else if(wait_change && ((in_mode(LINE) && (!in_mode(FUNCTION) || !in_function) && number_context_lines != 0) || (in_mode(FUNCTION) && in_function))) {

        if(in_mode(LINE) && (in_mode(FUNCTION) || !in_function) && length >= number_context_lines)
          additional_context.pop_front(), --length;

        additional_context.push_back(context);
        ++length;

      }

      if(code == COMMON_CODE || code == DELETE_CODE) ++line_number_delete;
      if(code == COMMON_CODE || code == INSERT_CODE) ++line_number_insert;

      context = "";

    }

  }

  if(code != COMMON_CODE) (*output) << COMMON_CODE;

}

/**
 * charactersUnit
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling within a unit.
 * Overide for desired behaviour.
 */
void bash_view::charactersUnit(const char * ch, int len) {

  if(diff_stack.back() != SESCOMMON) {

   output_additional_context();

   is_after_additional = false;
   is_after_change = false;
   wait_change = false;

   output->write(context.c_str(), context.size());
   context = "";

  }

  characters(ch, len);

  if(diff_stack.back() != SESCOMMON) is_after_change  = true;

}

