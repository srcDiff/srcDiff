#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

#include <fstream>

class ColorDiff {

private :

  bool first;

  xmlBuffer * srcdiff;
  std::ostream * outfile;

public:

  ColorDiff(xmlBuffer * srcdiff, const char * colordiff_file);
  ~ColorDiff();

  xmlBuffer * getsrcDiffBuffer();
  void setsrcDiffBuffer(xmlBuffer * srcdiff_buffer);

  int colorize(std::string file_one, std::string file_two);

};

#endif
