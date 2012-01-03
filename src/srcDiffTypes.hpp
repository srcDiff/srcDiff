#ifndef INCLUDED_SRCDIFFTYPES_HPP
#define INCLUDED_SRCDIFFTYPES_HPP

#include <vector>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <Options.hpp>

#include "Methods.hpp"

#include "xmlrw.hpp"

struct diff_set {

  int operation;

  std::vector<const xNode *> open_tags;

};

struct diff_nodes {

  std::vector<xNode *> & nodes_old;
  std::vector<xNode *> & nodes_new;

};

// stores information on state of a single file
class reader_state {

public:

  reader_state(int source) 
    : stream_source(source), last_output(0) { }

  void clear() {

    last_output = 0;
    nodes.clear();
    open_diff.clear();

  }

  int stream_source;
  unsigned int last_output;

  std::vector<xNode *> nodes;

  std::vector<diff_set *> open_diff;

};

// stores information during xml Text Writer processing
class writer_state {

public:

  void clear() {

    output_diff.clear();

  }


  xmlTextWriterPtr writer;

  METHOD_TYPE method;

  std::vector<diff_set *> output_diff;

};

#endif
