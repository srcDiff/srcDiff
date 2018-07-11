#include <diffdoc_view.hpp>

#include <srcdiff_constants.hpp>

#include <type_query.hpp>

#include <cstring>
#include <cassert>

diffdoc_view::diffdoc_view(const std::string & output_filename,
                           const std::string & syntax_highlight,
                           const std::string & theme,
                           bool ignore_all_whitespace,
                           bool ignore_whitespace,
                           bool ignore_comments,
                           bool is_html)
              : view_t(output_filename,
                       syntax_highlight, 
                       theme,
                       ignore_all_whitespace,
                       ignore_whitespace,
                       ignore_comments,
                       true) {

}

diffdoc_view::~diffdoc_view() {}

void diffdoc_view::reset_internal() {
}


void diffdoc_view::output_characters(const std::string & ch, int operation) {

  //output_characters_to_buffer(*output, ch, operation, last_character_operation, close_num_spans);

}

void diffdoc_view::start_unit(const std::string & local_name,
                              const char * prefix,
                              const char * URI,
                              int num_namespaces,
                              const struct srcsax_namespace * namespaces,
                              int num_attributes,
                              const struct srcsax_attribute * attributes) {

  (*output) << "<pre>";

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

  }

}

void diffdoc_view::end_unit(const std::string & local_name,
                            const char * prefix,
                            const char * URI) {


  (*output) << "</pre>";

}

void diffdoc_view::end_element(const std::string & local_name,
                               const char * prefix,
                               const char * URI) {

    if(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF) {

      if(local_name == "common" || local_name == "delete" || local_name == "insert")
        diff_stack.pop_back();

  } else {

  }

}

void diffdoc_view::characters(const char * ch, int len) {

}
