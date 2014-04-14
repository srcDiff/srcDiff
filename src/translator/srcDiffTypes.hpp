#ifndef INCLUDED_SRCDIFFTYPES_HPP
#define INCLUDED_SRCDIFFTYPES_HPP

#include <vector>
#include <string>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <Options.hpp>

#include <Methods.hpp>

#include <xmlrw.hpp>

#include <pthread.h>

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

  pthread_mutex_t * mutex;

  std::vector<xNode *> nodes;

  std::vector<diff_set *> open_diff;

};

// stores information during xml Text Writer processing
class writer_state {

public:

  void clear() {

    output_diff.clear();

  }

  std::string filename;
  xmlBufferPtr buffer;
  xmlTextWriterPtr writer;
  srcml_archive * archive;

  METHOD_TYPE method;

  std::vector<diff_set *> output_diff;

};

const int MAX_INT = (unsigned)-1 >> 1;

typedef std::vector<int> NodeSet;
typedef std::vector<NodeSet *> NodeSets;

typedef std::pair<int, int> IntPair;
typedef std::vector<IntPair> IntPairs;

#endif
