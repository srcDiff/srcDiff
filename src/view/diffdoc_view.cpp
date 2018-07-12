#include <diffdoc_view.hpp>

#include <srcdiff_constants.hpp>

#include <type_query.hpp>

#include <iomanip>

#include <cstring>
#include <cassert>

entity_data::entity_data(const std::string & type, size_t depth, size_t line_number_delete, size_t line_number_insert) 
  : type(type),
    depth(depth),
    line_number_delete(line_number_delete),
    line_number_insert(line_number_insert),
    collect_id(true),
    id(),
    signature(),
    is_changed(false) {}

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
                       saved_output(),
                       entity_stack() {}

diffdoc_view::~diffdoc_view() {}

void diffdoc_view::reset_internal() {
  num_open_spans = 0;
  last_character_operation = view_t::UNSET;
  line_number_delete = 1;
  line_number_insert = 1;
  saved_output = std::stack<std::ostringstream>();
  entity_stack = std::vector<entity_data>();
}

std::ostream * diffdoc_view::get_output_stream() {
  std::ostream * out = output;
  if(saved_output.size()) {
    out = &saved_output.top();
  }
  return out;
}

void diffdoc_view::add_saved_output() {
  saved_output.push(std::ostringstream());
}

std::string diffdoc_view::remove_saved_output() {
  std::string str = saved_output.top().str();
  saved_output.pop();
  return str;
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

std::string diffdoc_view::form_line_str(size_t original_line, size_t modified_line) {
  std::ostringstream out;
  std::string line_number_str = std::to_string(original_line) + '-' + std::to_string(modified_line);
  out << "<span style=\"color: " + theme->line_number_color + ";\">" 
      << std::right << std::setw(9) << std::setfill(' ') << line_number_str << "</span> ";
  return out.str();
}

void diffdoc_view::start_line() {
  output_raw_str(form_line_str(line_number_delete, line_number_insert));
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

    if(is_class_type(local_name) || is_function_type(local_name)) {
      end_spans();
      add_saved_output();
      entity_stack.emplace_back(local_name, srcml_element_stack.size(), line_number_delete, line_number_insert);
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

    if(is_class_type(local_name) || is_function_type(local_name)) {

      /** @todo will need to add class name to namespace and handle inner class/functions.
        Need to do variable.
      */
      /** gonna have to store old/new and have both so can walk through always use new to match next round or old for previous */
      /** want line number as part of this. Will have to buffer start of line until  or just store duplicate hidden*/
      /** setting display:none will make body disappear */

      end_spans();
      std::string body = remove_saved_output();

      output_raw_str("<span id=\"" + entity_stack.back().id + "\"");
      if(entity_stack.back().is_changed) {
        output_raw_str(" changed=\"changed\"");
      }
      output_raw_str(">");

      output_raw_str("<span signature=\"signature\">"); 
      output_raw_str(entity_stack.back().signature);
      output_raw_str("</span>");

      output_raw_str("<span body=\"body\">");
      output_raw_str(body);
      output_raw_str("</span>");
      output_raw_str("</span>");
      entity_stack.pop_back();

    } else if(entity_stack.size() && entity_stack.back().collect_id) {
      const std::string & type = entity_stack.back().type;
      bool end_id = (is_function_type(type) && local_name == "parameter_list") 
                 || (is_class_type(type) && entity_stack.back().depth == srcml_element_stack.size() && local_name == "name");
      if(end_id) {
        entity_stack.back().collect_id = false;
        end_spans();
        entity_stack.back().signature = remove_saved_output();
        add_saved_output();
      }

    }

  }

}

void diffdoc_view::characters(const char * ch, int len) {

  for(int i = 0; i < len; ++i) {

    if(diff_stack.back() != view_t::COMMON
      && entity_stack.size() && !entity_stack.back().is_changed) {

      typedef std::vector<entity_data>::reverse_iterator entity_ritr;
      for(entity_ritr ritr = entity_stack.rbegin(); ritr != entity_stack.rend() && !ritr->is_changed; ++ritr) {
        ritr->is_changed = true;
      }

    }

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
      if(entity_stack.size() && entity_stack.back().collect_id && srcml_element_stack.back() != "comment") {
        if(!is_space) {
          entity_stack.back().id += str;
        }
      }
    }

  }


}
