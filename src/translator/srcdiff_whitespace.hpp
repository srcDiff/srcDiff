#ifndef INCLUDED_SRCDIFF_WHITESPACE_HPP
#define INCLUDED_SRCDIFF_WHITESPACE_HPP

#include <srcdiff_output.hpp>

class srcdiff_whitespace : public srcdiff_output {

protected:

private:

  static const boost::optional<std::string> whitespace;
  void markup_whitespace(unsigned int end_original, unsigned int end_modified);

public:

  srcdiff_whitespace(const srcdiff_output & out);

  virtual void output_all();
  virtual void output_prefix();
  virtual void output_nested(int operation);
  virtual void output_statement();

  virtual void output_suffix();


};

#endif
