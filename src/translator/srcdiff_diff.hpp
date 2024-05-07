#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <srcdiff_output.hpp>
#include <construct.hpp>

#include <vector>

class srcdiff_diff {

protected:
  std::shared_ptr<srcdiff_output> out;

  const construct::construct_list & original;
  const construct::construct_list & modified;

public:
  srcdiff_diff(std::shared_ptr<srcdiff_output> out, const construct::construct_list & original, const construct::construct_list & _modified);

  virtual void output();
  virtual void output_pure(int end_original, int end_modified);
  virtual void output_change_whitespace(int end_original, int end_modified);
  virtual void output_replace_inner_whitespace(int start_original, int end_original,
                                               int start_modified, int end_modified,
                                               int common_offset);
};

#endif
