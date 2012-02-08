#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

#include <fstream>

class ColorDiff {

public :

  bool first;

  xmlBuffer * srcdiff;
  std::ofstream * outfile;

public:

  ColorDiff(xmlBuffer * srcdiff, const char * colordiff_file);
  ~ColorDiff();

  int colorize(const char * file_one, const char * file_two);

};

#endif
