#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

#include <fstream>

class ColorDiff {

public :

  xmlBuffer * srcdiff;
  
  std::ofstream outfile;

  ColorDiff(xmlBuffer * srcdiff, const char * outfile);

  int colorize(const char * file_one, const char * file_two);

};

#endif
