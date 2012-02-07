#ifndef INCLUDED_COLORDIFF_HPP
#define INCLUDED_COLORDIFF_HPP

class ColorDiff {

public :

  xmlBuffer * srcdiff;
  
  const char * outfile;

  ColorDiff() { }

  ColorDiff(xmlBuffer * srcdiff, const char * outfile);

  int colorize(const char * file_one, const char * file_two);

};

#endif
