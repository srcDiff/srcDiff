/*
  bash_view.cpp

  Michael John Decker
  mdecker6@kent.edu
*/

#ifndef INCLUDED_SAX2BASHVIEW_HPP
#define INCLUDED_SAX2BASHVIEW_HPP

#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <iostream>

#include <libxml/parser.h>

class bash_view {

private:

  std::vector<int> diff_stack;

  std::ostream * output;

  unsigned long line_number;

  std::string context;

  bool is_line_output;
  bool is_after_change;

  unsigned long num_context_lines;
  std::list<std::string> additional_context;

  bool is_after_additional;

public:

  bash_view(const std::string & output_filename, unsigned long num_context_lines) : line_number(0), is_line_output(false), is_after_change(false),
    num_context_lines(num_context_lines), is_after_additional(false) {

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

  int transform(const char * srcdiff);

  static xmlSAXHandler factory();

  static void startDocument(void* ctx);

  static void endDocument(void* ctx);


  static void startElementNs(void* ctx, const xmlChar* localname, const xmlChar* prefix, const xmlChar* URI,
                      int nb_namespaces, const xmlChar** namespaces, int nb_attributes, int nb_defaulted,
                      const xmlChar** attributes);

  static void endElementNs(void *ctx, const xmlChar *localname, const xmlChar *prefix, const xmlChar *URI);

  void process_characters(const char * ch, int len);
  void output_additional_context();

  static void characters(void* ctx, const xmlChar* ch, int len);

  static void comment(void* ctx, const xmlChar* ch);

};

#endif
