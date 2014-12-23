#ifndef INCLUDED_SRCDIFF_OUTPUT_HPP
#define INCLUDED_SRCDIFF_OUTPUT_HPP


#include <ColorDiff.hpp>
#include <bash_view.hpp>

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

    while(!open_diff.empty()) {

      delete open_diff.back();
      open_diff.pop_back();

    }

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

    while(!output_diff.empty()) {

      delete output_diff.back();
      output_diff.pop_back();

    }

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

  srcml_archive * archive;
  std::shared_ptr<ColorDiff> colordiff;
  std::shared_ptr<bash_view> bashview;
  OPTION_TYPE options;

  std::shared_ptr<reader_state> rbuf_old;
  std::shared_ptr<reader_state> rbuf_new;
  std::shared_ptr<writer_state> wstate;

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

  static void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation);

public:

  srcdiff_output(srcml_archive * archive, const char * srcdiff_filename, OPTION_TYPE options, METHOD_TYPE method, const char * prefix,
    std::string css, unsigned long number_context_lines);
  virtual ~srcdiff_output();

  virtual void initialize(int is_old, int is_new);
  virtual void start_unit(const char * language_string, const char * unit_directory, const char * unit_filename, const char * unit_version);
  virtual void finish(int is_old, int is_new, LineDiffRange & line_diff_range);
  virtual void reset();
  virtual void close();

  virtual std::vector<xNodePtr> & get_nodes_old();
  virtual std::vector<xNodePtr> & get_nodes_new();
  virtual unsigned int & last_output_old();
  virtual unsigned int & last_output_new();
  METHOD_TYPE method() const;

  virtual void output_node(const xNodePtr node, int operation);
  virtual void output_text_as_node(const char * text, int operation);
  virtual void output_char(char character, int operation);

};

#endif
