#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

#include <fstream>
#include <srcdiff_options.hpp>
#include <LineDiffRange.hpp>

class ColorDiff {

private :

  bool first;

  std::string directory;
  std::string version;
  OPTION_TYPE & options;

  std::ostream * outfile;


public:

  ColorDiff(const std::string & colordiff_file, const std::string & directory, const std::string & version, OPTION_TYPE & options);
  ~ColorDiff();

  int colorize(const char * srcdiff, LineDiffRange & line_diff_range);

};

#endif
