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
  std::string css_url;
  OPTION_TYPE & options;

  std::ostream * outfile;


public:

  ColorDiff(std::string colordiff_file, std::string directory, std::string version, std::string css,
            OPTION_TYPE & options);
  ~ColorDiff();

  int colorize(const char * srcdiff, LineDiffRange & line_diff_range);

};

#endif
