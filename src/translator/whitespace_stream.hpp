// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file whitespace_stream.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_WHITESPACE_STREAM_HPP
#define INCLUDED_WHITESPACE_STREAM_HPP

#include <output_stream.hpp>

namespace srcdiff {

class whitespace_stream : public output_stream {

protected:

private:

  void markup_whitespace(unsigned int end_original, unsigned int end_modified);
  static int extend_end_to_new_line(std::shared_ptr<reader_state> rbuf);

public:

  static void output_whitespace(std::shared_ptr<output_stream> out) {
    whitespace_stream whitespace(*out);
    whitespace.output_all();
  }

  whitespace_stream(const output_stream& out);

  virtual void output_all(int operation = SES_COMMON);
  virtual void output_prefix();
  virtual void output_nested(int operation);
  virtual void output_statement();

  virtual void output_suffix();

};

}

#endif
