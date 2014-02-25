/*
  URIStream.cpp

  Copyright (C) 2010-2014  SDML (www.srcML.org)

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

#include "URIStream.hpp"
#include <cstring>

URIStream::URIStream(const char* uriname)
  : startpos(0), endpos(-1)/*, first(true)*/, eof(false), done(false)
{
  if (!(input = xmlParserInputBufferCreateFilename(uriname, XML_CHAR_ENCODING_NONE)))
    throw URIStreamFileError();

  // get some data into the buffer
  int size = xmlParserInputBufferGrow(input, 4096);

  // found problem or eof
  if (size == -1 || size == 0)
    done = true;
}


std::string URIStream::readlines() {

  std::string s;
  char* line = 0;
  while ((line = readline())) {
    s.append(line);
    s.append(" ");
  }

  return s;
}

char* URIStream::readline() {

  if (done)
    return 0;

  endpos = startpos;

  // find a line in the buffer
#ifdef LIBXML2_NEW_BUFFER
  while (xmlBufContent(input->buffer)[endpos] != '\n') {
#else
  while (input->buffer->content[endpos] != '\n') {
#endif
    ++endpos;

    // need to refill the buffer
#ifdef LIBXML2_NEW_BUFFER
    if (endpos >= xmlBufUse(input->buffer)) {
#else
    if (endpos >= input->buffer->use) {
#endif
      // need to refill, but previous fill found eof
      if (eof)
	break;

      // shrink the part of the buffer that we are not using yet
      // this is a large buffer, so this will not happen very often, and
      // only if libxml decides for this input source it should

#ifdef LIBXML2_NEW_BUFFER
      int removed = (int)xmlBufShrink(input->buffer, startpos >= 1 ? startpos - 1 : 0);
#else
      int removed = (int)xmlBufferShrink(input->buffer, startpos >= 1 ? startpos - 1 : 0);
#endif
      endpos -= removed;
      startpos -= removed;

      // refill the buffer
      // put an appropriate value for the length, but note that libxml
      // basically uses 4 or a min value (which is currently around 4096)
      int size = xmlParserInputBufferGrow(input, 4096);

      // found problem or eof
      if (size == -1 || size == 0)
	eof = true;
    }
  }

  // special case
#ifdef LIBXML2_NEW_BUFFER
  if (startpos >= xmlBufUse(input->buffer))
#else
  if (startpos >= input->buffer->use)
#endif
    return 0;

  // replace the linefeed, and the optional carriage return before it, with a null to turn it into single string
#ifdef LIBXML2_NEW_BUFFER
  if ((endpos >= 1) && (xmlBufContent(input->buffer)[endpos - 1] == '\r'))
    xmlBufContent(input->buffer)[endpos - 1] = '\0';
  xmlBufContent(input->buffer)[endpos] = '\0';
  // current line starts at the startpos
  char* line = (char*) xmlBufContent(input->buffer) + startpos;
#else
  if ((endpos >= 1) && (input->buffer->content[endpos - 1] == '\r'))
    input->buffer->content[endpos - 1] = '\0';
  input->buffer->content[endpos] = '\0';
  // current line starts at the startpos
  char* line = (char*) input->buffer->content + startpos;
#endif

  // skip past for the next line
  startpos = endpos + 1;

  return line;
}

URIStream::~URIStream() {

    xmlFreeParserInputBuffer(input);
}
