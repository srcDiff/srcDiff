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

#include <output_stream.hpp>

class srcdiff_whitespace : public srcdiff::output_stream {

protected:

private:

  void markup_whitespace(unsigned int end_original, unsigned int end_modified);
  static int extend_end_to_new_line(std::shared_ptr<reader_state> rbuf);

public:

  static void output_whitespace(std::shared_ptr<srcdiff::output_stream> out) {
    srcdiff_whitespace whitespace(*out);
    whitespace.output_all();
  }

  srcdiff_whitespace(const srcdiff::output_stream & out);

  virtual void output_all(int operation = SES_COMMON);
  virtual void output_prefix();
  virtual void output_nested(int operation);
  virtual void output_statement();

  virtual void output_suffix();


};

#endif
