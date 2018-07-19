#include <diffdoc_view.hpp>

#include <srcdiff_constants.hpp>

#include <type_query.hpp>
#include <summary_output_stream_html.hpp>
#include <summary_manip.hpp>

#include <iomanip>
#include <memory>

#include <cstring>
#include <cassert>

entity_data::entity_data(const std::string & type, size_t depth, 
                         const std::string & indentation,
                         size_t line_number_delete, size_t line_number_insert,
                         srcdiff_type operation)
  : type(type),
    depth(depth),
    indentation(indentation),
    line_number_delete(line_number_delete),
    line_number_insert(line_number_insert),
    operation(operation),
    collect_id(true),
    // not a good separtor for C++
    id('|'),
    signature(),
    collect_name(false),
    name(),
    is_modified(false),
    change_profile() {}



bool entity_data::is_changed() const {
  return is_modified || operation != SRCDIFF_COMMON;
}

diffdoc_view::diffdoc_view(const std::string & output_filename,
                           const std::string & syntax_highlight,
                           const std::string & theme,
                           std::shared_ptr<srcdiff_summary> & summarizer)
              : view_t(output_filename,
                       syntax_highlight, 
                       theme,
                       false,
                       false,
                       false,
                       true),
                summarizer(summarizer),
                num_open_spans(0),
                last_character_operation(view_t::UNSET),
                line_number_delete(1),
                line_number_insert(1),
                indentation(),
                saved_output(),
                entity_stack() {}

diffdoc_view::~diffdoc_view() {}

void diffdoc_view::transform(const std::string & srcdiff, const std::string & xml_encoding) {
  summarizer->perform_summary(srcdiff, xml_encoding);
  manip::set_is_html(true);
  view_t::transform(srcdiff, xml_encoding);
  manip::set_is_html(false);

}

void diffdoc_view::reset_internal() {
  summarizer->reset();
  num_open_spans = 0;
  last_character_operation = view_t::UNSET;
  line_number_delete = 1;
  line_number_insert = 1;
  indentation.clear();
  saved_output = std::stack<std::ostringstream>();
  entity_stack.clear();
}

srcdiff_type diffdoc_view::view_op2srcdiff_type(int operation) {
  static std::unordered_map<int, srcdiff_type> op_converter = {
    { view_t::UNSET,  SRCDIFF_NONE   },
    { view_t::COMMON, SRCDIFF_COMMON },
    { view_t::DELETE, SRCDIFF_DELETE },
    { view_t::INSERT, SRCDIFF_INSERT },
  };

  return op_converter[operation];

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
  indentation.clear();
  // if(diff_stack.back() != view_t::COMMON) {
  //     output_characters(CARRIAGE_RETURN_SYMBOL);   
  // }
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
  output_raw_str("<span filename=\"" + profile_t::unit_profile->get_name() + "\">");
  output_raw_str("</span>");
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
    /** @todo add static block and fields */
    if(is_class_type(local_name) || is_function_type(local_name)
      || (entity_stack.size() && is_class_type(entity_stack.back().type)
          && entity_stack.back().depth == (srcml_element_stack.size() - 2)
          && is_decl_stmt(local_name))) {
      end_spans();
      add_saved_output();
      entity_stack.emplace_back(local_name, srcml_element_stack.size(), 
                                indentation, line_number_delete, line_number_insert,
                                view_op2srcdiff_type(diff_stack.back()));
      indentation.clear();
    } else if(entity_stack.size() && entity_stack.back().collect_id && is_identifier(local_name)) {

      size_t start_depth = is_decl_stmt(entity_stack.back().type) ? srcml_element_stack.size() - 2 : srcml_element_stack.size() - 1;
      if(entity_stack.back().depth == start_depth) {
        entity_stack.back().collect_name = true;
      }

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

    if(is_class_type(local_name) || is_function_type(local_name)
      || (entity_stack.size() && is_decl_stmt(entity_stack.back().type)
          && is_decl_stmt(local_name))) {

      /** @todo will need to add class name to namespace and handle inner class/functions.
        Need to do variable.
      */
      /** setting display:none will make body disappear */

      end_spans();

      std::string changed_value = "";
      if(entity_stack.back().is_changed()) {
        switch(entity_stack.back().operation) {
          case SRCDIFF_COMMON: changed_value = "modified"; break;
          case SRCDIFF_DELETE: changed_value = "deleted";  break;
          case SRCDIFF_INSERT: changed_value = "inserted"; break;
          default: break;
        }
      }

      std::string body = remove_saved_output();
      std::string id_attr = "id=\"" + entity_stack.back().id + "\"";
      output_raw_str("<span " + id_attr + " content=\"full\"");
      if(entity_stack.back().is_changed()) {
        output_raw_str(" changed=\"" + changed_value + "\"");
      }
      output_raw_str(">");

      if(entity_stack.back().change_profile) {
        std::ostringstream out;
        summary_output_stream_html stream(out);
        entity_stack.back().change_profile->summary(stream, summary_type::TEXT);
        stream.finish();

        output_raw_str("<span " + id_attr + " content=\"summary\" style=\"display:none\">");
        output_raw_str(out.str());
        output_raw_str("</span>");
      }

      output_raw_str("<span " + id_attr + " content=\"signature\""); 
      if(entity_stack.back().is_changed()) {
        output_raw_str(" changed=\"" + changed_value + "\"");
      }
      output_raw_str(">");

      output_raw_str("<span " + id_attr + " content=\"pre\" style=\"display:none\">" 
        + form_line_str(entity_stack.back().line_number_delete, entity_stack.back().line_number_insert) 
        + entity_stack.back().indentation + "</span>");
      output_raw_str(entity_stack.back().signature);
      output_raw_str("</span>");

      output_raw_str("<span " + id_attr + " content=\"body\">");
      output_raw_str(body);
      output_raw_str("</span>");
      output_raw_str("</span>");
      entity_stack.pop_back();

    } else if(entity_stack.size() && entity_stack.back().collect_id) {
      const std::string & type = entity_stack.back().type;

      size_t end_depth = is_decl_stmt(type) ? srcml_element_stack.size() - 1 : srcml_element_stack.size();
      if(is_identifier(local_name) && entity_stack.back().depth == end_depth) {
        entity_stack.back().collect_name = false;
        if(is_function_type(entity_stack.back().type)) {
          set_change_profile_by_name<function_profile_t>();
        } else if(is_class_type(entity_stack.back().type)) {
          set_change_profile_by_name<class_profile_t>();
        } else if(is_decl_stmt(entity_stack.back().type)) {
          set_change_profile_by_name<decl_stmt_profile_t>();
        }
      }

      bool end_func   = is_function_type(type) && local_name == "parameter_list";
      bool end_class  = is_class_type(type) && entity_stack.back().depth == end_depth              
                        && is_identifier(local_name);
      bool end_member = is_decl_stmt(type) && entity_stack.back().depth == end_depth              
                        && is_identifier(local_name);
      bool end_id = end_func || end_class || end_member;

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
      && entity_stack.size() && !entity_stack.back().is_modified) {

      typedef std::vector<entity_data>::reverse_iterator entity_ritr;
      for(entity_ritr ritr = entity_stack.rbegin(); ritr != entity_stack.rend() && !ritr->is_modified; ++ritr) {
        ritr->is_modified = true;
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

      if(entity_stack.size() && entity_stack.back().collect_id && !is_comment(srcml_element_stack.back())) {
        entity_stack.back().id.append(" ", view_op2srcdiff_type(diff_stack.back()));
      }

    } else {

      output_characters(str);

      if(entity_stack.size() && entity_stack.back().collect_id && !is_comment(srcml_element_stack.back())) {
        std::string id;
        for(char ch : str) {
          if(ch != '"') id.append(1, ch);
          else          id.append("&quot;");
        }
        entity_stack.back().id.append(id, view_op2srcdiff_type(diff_stack.back()));
      }

      if(is_space) {
        // do I also store how it is deleted/inserted and/or not whitespace?
        indentation.append(str);
      } else {
        indentation.clear();
      }
    }

    if(entity_stack.size() && entity_stack.back().collect_name) {
      entity_stack.back().name.append(str, view_op2srcdiff_type(diff_stack.back()));
    }

  }


}
