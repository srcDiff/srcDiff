#include <diffdoc_view.hpp>

#include <srcdiff_constants.hpp>

#include <type_query.hpp>

#include <iomanip>

#include <cstring>
#include <cassert>

diffdoc_view::diffdoc_view(const std::string & output_filename,
                           const std::string & syntax_highlight,
                           const std::string & theme)
              : view_t(output_filename,
                       syntax_highlight, 
                       theme,
                       false,
                       false,
                       false,
                       true),
                       num_open_spans(0),
                       last_character_operation(view_t::UNSET),
                       line_number_delete(1),
                       line_number_insert(1),
                       save_output(false),
                       saved_output(),
                       collect_id(),
                       id() {}

diffdoc_view::~diffdoc_view() {}

void diffdoc_view::reset_internal() {
  num_open_spans = 0;
  last_character_operation = view_t::UNSET;
  line_number_delete = 1;
  line_number_insert = 1;
  save_output = false;
  saved_output = std::ostringstream();
  collect_id = false;
  id = std::string();
}

std::ostream * diffdoc_view::get_output_stream() {
  std::ostream * out = output;
  if(save_output) {
    out = &saved_output;
  }
  return out;
}


void diffdoc_view::enable_saving() {
  save_output = true;
}

void diffdoc_view::disable_saving() {
  save_output = false;
}

void diffdoc_view::output_saved() {
  disable_saving();
  output_raw_str(saved_output.str());
  saved_output = std::ostringstream();
}

void diffdoc_view::end_spans() {
  end_buffer(*get_output_stream(), num_open_spans);
  last_character_operation = view_t::UNSET;
}

void diffdoc_view::output_raw_str(const std::string & str) {
  (*get_output_stream()) << str;
}

void diffdoc_view::output_characters(const std::string & str) {
  output_characters(str, diff_stack.back());
}

void diffdoc_view::output_characters(const std::string & str, int operation) {
  output_characters_to_buffer(*get_output_stream(), str, operation, last_character_operation, num_open_spans);
}

void diffdoc_view::start_line() {
  std::ostringstream out;
  out << "<span style=\"color: " + theme->line_number_color + ";\">" 
    << std::right << std::setw(9) << std::setfill(' ') << line_number_delete << '-' << line_number_insert << "</span> ";
  output_raw_str(out.str());
}

void diffdoc_view::end_line() {
  if(diff_stack.back() != view_t::COMMON) {
      output_characters(CARRIAGE_RETURN_SYMBOL);   
  }
  output_characters("\n", view_t::COMMON);
  end_spans();
  if(diff_stack.back() != INSERT) ++line_number_delete;
  if(diff_stack.back() != DELETE) ++line_number_insert;

}

void diffdoc_view::start_unit(const std::string & local_name,
                              const char * prefix,
                              const char * URI,
                              int num_namespaces,
                              const struct srcsax_namespace * namespaces,
                              int num_attributes,
                              const struct srcsax_attribute * attributes) {

  output_raw_str("<pre>");
  start_line();

}

void diffdoc_view::start_element(const std::string & local_name,
                                 const char * prefix,
                                 const char * URI,
                                 int num_namespaces,
                                 const struct srcsax_namespace * namespaces,
                                 int num_attributes,
                                 const struct srcsax_attribute * attributes) {


  if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

    if(local_name == "common")
     diff_stack.push_back(view_t::COMMON);
    else if(local_name == "delete")
     diff_stack.push_back(DELETE);
    else if(local_name == "insert")
     diff_stack.push_back(INSERT);
    
  } else {

    if(is_function_type(local_name)) {
      end_spans();
      enable_saving();
      collect_id = true;
    }

  }

}

void diffdoc_view::end_unit(const std::string & local_name,
                            const char * prefix,
                            const char * URI) {

  end_spans();
  output_raw_str("</pre>");

}

void diffdoc_view::end_element(const std::string & local_name,
                               const char * prefix,
                               const char * URI) {

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(local_name == "common" || local_name == "delete" || local_name == "insert")
        diff_stack.pop_back();

  } else {

    if(is_function_type(local_name)) {

      /** @todo will need to add class name to namespace and handle inner class/functions.
        Need to do variable.
      */
      end_spans();
      output_raw_str("</span>");

    } else if(collect_id && local_name == "parameter_list") {
      collect_id = false;
      end_spans();
      disable_saving();

      output_raw_str("<span id=\"" + id + "\">"); 
      id = std::string();
      output_saved();

    }

  }

}

void diffdoc_view::characters(const char * ch, int len) {

  for(int i = 0; i < len; ++i) {

    bool is_space = isspace(ch[i]);
    std::string str(1, ch[i]);

    if(!is_space || ch[i] != '\n') {

      ++i;
      while(i < len && isspace(ch[i]) == is_space && ch[i] != '\n') {
        str += ch[i];
        ++i;
      }
      --i;

    }

    if(str == "\n") {
      end_line();
      start_line();
    } else {
      output_characters(str);
      if(collect_id && srcml_element_stack.back() != "comment") {
        if(is_space) {
          id += '-';
        } else {
          id += str;
        }
      }
    }

  }


}
