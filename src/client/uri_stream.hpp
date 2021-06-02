/*
  URIStream.hpp

  Copyright (C) 2010-2021  SDML (www.srcML.org)

  This file is part of the srcML Toolkit.

  The srcML Toolkit is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML Toolkit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML Toolkit; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
