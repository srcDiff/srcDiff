#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

#include <fstream>

class ColorDiff {

private :

  bool first;

  xmlBuffer * srcdiff;
  std::string css_url;

  std::ostream * outfile;


public:

  ColorDiff(xmlBuffer * srcdiff, std::string colordiff_file, std::string css);
  ~ColorDiff();

  xmlBuffer * getsrcDiffBuffer();
  void setsrcDiffBuffer(xmlBuffer * srcdiff_buffer);

  int colorize(std::string file_one, std::string file_two);

};

#endif
