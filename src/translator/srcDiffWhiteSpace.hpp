#ifndef INCLUDED_SRCDIFF_WHITESPACE_HPP
#define INCLUDED_SRCDIFF_WHITESPACE_HPP

#include <srcDiffOutput.hpp>
class srcdiff_whitespace : public srcdiff_output {

protected:

private:

  void markup_whitespace(unsigned int end_old, unsigned int end_new);

public:

  srcdiff_whitespace(const srcdiff_output & out);

  virtual void output_white_space_all();
  virtual void output_white_space_prefix();
  virtual void output_white_space_nested(int operation);
  virtual void output_white_space_statement();

  virtual void output_white_space_suffix();


};

#endif
