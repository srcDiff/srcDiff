#ifndef INCLUDED_SRCDIFFOUTPUT_HPP
#define INCLUDED_SRCDIFFOUTPUT_HPP

#include <methods.hpp>
#include <xmlrw.hpp>
#include <vector>
#include <pthread.h>
#include <memory>

// const nodes here? or xmlrw
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

    for(unsigned int i = 0; i < nodes.size(); ++i) {

      if(nodes.at(i)->free) {

        freeXNode(nodes[i]);

      }

    }

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
  srcml_unit * unit;

  METHOD_TYPE method;

  std::vector<diff_set *> output_diff;

};

class srcdiff_output {

protected:

  std::shared_ptr<reader_state> rbuf_old;
  std::shared_ptr<reader_state> rbuf_new;
  std::shared_ptr<writer_state> wstate;

  pthread_mutex_t mutex;

public:

  // diff nodes
  std::shared_ptr<xNode> diff_common_start;
  std::shared_ptr<xNode> diff_common_end;
  std::shared_ptr<xNode> diff_old_start;
  std::shared_ptr<xNode> diff_old_end;
  std::shared_ptr<xNode> diff_new_start;
  std::shared_ptr<xNode> diff_new_end;

  std::shared_ptr<xNs> diff;

  // diff attribute
  std::shared_ptr<xAttr> diff_type;

  std::shared_ptr<xNode> unit_tag;

private:

public:

  srcdiff_output(const char * srcdiff_filename, METHOD_TYPE method, const char * prefix);
  virtual ~srcdiff_output();
  virtual void flush();
  virtual void reset();

  virtual reader_state & get_rbuf_old();
  virtual reader_state & get_rbuf_new();
  virtual writer_state & get_wstate();

  virtual void output_node(const xNodePtr node, int operation);
  virtual void output_text_as_node(const char * text, int operation);
  virtual void output_char(char character, int operation);

  static void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation);

};

#endif
