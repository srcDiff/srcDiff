// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file uri_stream.hpp
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <libxml/parser.h>
#include <string>

#ifndef INCLUDED_URI_STREAM_HPP
#define INCLUDED_URI_STREAM_HPP

class uri_stream_error {};

template<class T>
class uri_stream {

  private:

    xmlParserInputBufferPtr input;

    unsigned int startpos;
    unsigned int endpos;
    bool eof;
    bool done;

  public:

    uri_stream(typename T::input_context * context);

    ~uri_stream();

    char* readline();

    std::string readlines();

};

#include <uri_stream.tcc>

#endif
