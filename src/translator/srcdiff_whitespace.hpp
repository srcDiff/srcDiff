// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_whitespace.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_WHITESPACE_HPP
#define INCLUDED_SRCDIFF_WHITESPACE_HPP

#include <srcdiff_output.hpp>

class srcdiff_whitespace : public srcdiff_output {

protected:

private:

  void markup_whitespace(unsigned int end_original, unsigned int end_modified);
  static bool extend_end_to_new_line(std::shared_ptr<reader_state> rbuf, unsigned int& end);

public:

  static void output_whitespace(std::shared_ptr<srcdiff_output> out) {
    srcdiff_whitespace whitespace(*out);
    whitespace.output_all();
  }

  srcdiff_whitespace(const srcdiff_output & out);

  virtual void output_all(int operation = SES_COMMON);
  virtual void output_prefix();
  virtual void output_nested(int operation);

  virtual void output_suffix(int operation);


};

#endif
