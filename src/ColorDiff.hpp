#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

#include <fstream>
#include <Options.hpp>
#include "LineDiffRange.hpp"

class ColorDiff {

private :

  bool first;

  xmlBuffer * srcdiff;
  std::string directory;
  std::string version;
  std::string css_url;
  OPTION_TYPE & options;

  std::ostream * outfile;


public:

  ColorDiff(LineDiffRange & line_diff_range, xmlBuffer * srcdiff, std::string colordiff_file, std::string directory, std::string version, std::string css,
            OPTION_TYPE & options);
  ~ColorDiff();

  xmlBuffer * getsrcDiffBuffer();
  void setsrcDiffBuffer(xmlBuffer * srcdiff_buffer);

  int colorize(std::string file_one, std::string file_two);

};

#endif
