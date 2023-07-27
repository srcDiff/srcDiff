#ifndef INCLUDED_SRCDIFF_DIFF_HPP
#define INCLUDED_SRCDIFF_DIFF_HPP

#include <srcdiff_output.hpp>
#include <construct_list.hpp>
#include <shortest_edit_script.h>

#include <vector>

class srcdiff_diff {

protected:
  srcdiff_output & out;

  const construct_list & construct_list_original;
  const construct_list & construct_list_modified;

public:
  srcdiff_diff(srcdiff_output & out, const construct_list & construct_list_original, const construct_list & construct_list_modified);

  virtual void output();
  virtual void output_common(int end_original, int end_modified);
  virtual void output_pure(int end_original, int end_modified);
  virtual void output_change(int end_original, int end_modified);
  virtual void output_whitespace();
  virtual void output_change_whitespace(int end_original, int end_modified);
  virtual void output_replace_inner_whitespace(int start_original, int end_original,
                                               int start_modified, int end_modified,
                                               int common_offset);
};

#endif
