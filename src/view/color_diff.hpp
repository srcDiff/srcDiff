#ifndef INCLUDED_COLOR_DIFF_HPP
#define INCLUDED_COLOR_DIFF_HPP

#include <fstream>
#include <srcdiff_options.hpp>
#include <line_diff_range.hpp>

class color_diff {

private :

  bool first;

  std::string directory;
  std::string version;
  const OPTION_TYPE & options;

  std::ostream * outfile;


public:

  color_diff(const std::string & color_diff_file, const std::string & directory, const std::string & version, const OPTION_TYPE & options);
  ~color_diff();

  template<class T>
  int colorize(const char * srcdiff, line_diff_range<T> & line_diff_range);

};

#include <libxml/parserInternals.h>
#include <sax2_color_diff.hpp>

xmlParserCtxtPtr create_url_parser_ctxt(const char * srcdiff);
void parse_document(xmlParserCtxtPtr ctxt);

template<class T>
int color_diff::colorize(const char * srcdiff, line_diff_range<T> & line_diff_range) {

  unsigned int size_original = line_diff_range.get_length_file_one();
  unsigned int size_modified = line_diff_range.get_length_file_two();

  std::string stream = line_diff_range.get_line_diff_range();

  std::vector<bool> lines_original;
  std::vector<bool> lines_modified;

  std::stringstream in(stream);

  char operation;
  while(in >> operation) {

    unsigned int start;
    in >> start;

    char temp;
    in >> temp;

    unsigned int end;
    in >> end;

    if(operation == 'd') {

      while(lines_original.size() < start) {
        lines_original.push_back(false);
      }

      while(lines_original.size() <= end) {
        lines_original.push_back(true);
      }

    } else {

      while(lines_modified.size() < start) {
        lines_modified.push_back(false);
      }

      while(lines_modified.size() <= end) {
        lines_modified.push_back(true);
      }

    }

  }

  if(!is_option(options, OPTION_SAME) && lines_original.size() == 0 && lines_modified.size() == 0)
    return 0;

  if(!is_option(options, OPTION_PURE) && (size_original == 0 || size_modified == 0))
    return 0;

  while(lines_original.size() <= size_original) {
        lines_original.push_back(false);
  }

  while(lines_modified.size() <= size_modified) {
        lines_modified.push_back(false);
  }

  if(first) {

    sax2_color_diff::output_start_document(*outfile);
    first = false;

  }

  // create the ctxt
  xmlParserCtxtPtr ctxt = create_url_parser_ctxt(srcdiff);

  // setup sax handler
  xmlSAXHandler sax = sax2_color_diff::factory();
  ctxt->sax = &sax;

  std::vector<int> stack = std::vector<int>();
  stack.push_back(SES_COMMON);

  struct sax2_color_diff::source_diff data = { 1, 1, &stack, lines_original, lines_modified, line_diff_range.get_file_one(), line_diff_range.get_file_two()
                              , *outfile, options, "", false, false, false };

  ctxt->_private = &data;

  parse_document(ctxt);

  // local variable, do not want xmlFreeParserCtxt to free
  ctxt->sax = NULL;

  // all done with parsing
  xmlFreeParserCtxt(ctxt);

  return 0;
}

#endif
