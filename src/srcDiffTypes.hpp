#ifndef INCLUDED_SRCDIFFTYPES_HPP
#define INCLUDED_SRCDIFFTYPES_HPP

#include <vector>

struct diff_set {

  int operation;

  std::vector<xmlNodePtr> open_tags;

};

// stores information on state of a single file
struct reader_state {

  int stream_source;
  unsigned int last_output;

  // just a pointer not on stack
  std::vector<diff_set *> open_diff;

};

// stores information during xml Text Writer processing
struct writer_state {

  xmlTextWriterPtr writer;
  std::vector<diff_set *> output_diff;

};

#endif
