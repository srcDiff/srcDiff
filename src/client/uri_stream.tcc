// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file uri_stream.tcc
 *
 * @copyright Copyright (C) 2015-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <cstring>

template<class T>
int uri_stream_read(void * context, char * buffer, int len) {

  return T::read(context, buffer, (size_t)len);

}

template<class T>
int uri_stream_close(void * context) {

  return T::close(context);

}


template<class T>
uri_stream<T>::uri_stream(typename T::input_context * context)
  : startpos(0), endpos(-1), eof(false), done(false)
{

  if (!(input = xmlParserInputBufferCreateIO(uri_stream_read<T>, uri_stream_close<T>, context, XML_CHAR_ENCODING_NONE)))
    throw uri_stream_error();

  // get some data into the buffer
  int size = xmlParserInputBufferGrow(input, 4096);

  // found problem or eof
  if (size == -1 || size == 0) {
    done = true;
    }
}

template<class T>
uri_stream<T>::~uri_stream() {

    xmlFreeParserInputBuffer(input);

}

template<class T>
std::string uri_stream<T>::readlines() {

  std::string s;
  char* line = 0;
  while ((line = readline())) {
    s.append(line);
    s.append(" ");
  }

  return s;
}

template<class T>
char * uri_stream<T>::readline() {

  if (done)
    return 0;

  endpos = startpos;

  #ifdef LIBXML2_NEW_BUFFER
    if (endpos >= xmlBufUse(input->buffer)) {
#else
    if (endpos >= input->buffer->use) {
#endif

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
    if (size == -1 || size == 0) {
      eof = true;
      }
  }

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
      if (size == -1 || size == 0) {
	eof = true;
	}
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
  if ((endpos >= 1) && (input->buffer->content[endpos - 1] == '\r')) {
    input->buffer->content[endpos - 1] = '\0';
  }
  input->buffer->content[endpos] = '\0';
  // current line starts at the startpos
  char* line = (char*) input->buffer->content + startpos;
#endif

  // skip past for the next line
  startpos = endpos + 1;

  return line;
}
