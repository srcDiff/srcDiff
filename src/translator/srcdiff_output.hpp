#ifndef INCLUDED_SRCDIFF_OUTPUT_HPP
#define INCLUDED_SRCDIFF_OUTPUT_HPP

#include <srcml_node.hpp>
#include <LineDiffRange.hpp>
#include <ColorDiff.hpp>
#include <bash_view.hpp>
#include <methods.hpp>

#include <vector>
#include <memory>

class srcdiff_output {

protected:

  struct diff_set {

    int operation;

    std::vector<const srcml_node *> open_tags;

  };

  // stores information on state of a single file
  class reader_state {

  public:

    reader_state(int source) 
      : stream_source(source), last_output(0) { }

    void clear() {

      for(unsigned int i = 0; i < nodes.size(); ++i) {

        if(nodes.at(i)->free) {

          delete nodes[i];

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

    std::vector<srcml_node *> nodes;

    std::vector<diff_set *> open_diff;

  };

  // stores information during xml Text Writer processing
  class writer_state {

  public:

    writer_state(const METHOD_TYPE & method) : method(method) {}

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

    const METHOD_TYPE & method;

    std::vector<diff_set *> output_diff;

  };

  srcml_archive * archive;
  std::shared_ptr<ColorDiff> colordiff;
  std::shared_ptr<bash_view> bashview;
  const OPTION_TYPE & flags;

  std::shared_ptr<reader_state> rbuf_old;
  std::shared_ptr<reader_state> rbuf_new;
  std::shared_ptr<writer_state> wstate;

public:

  // diff nodes
  std::shared_ptr<srcml_node> diff_common_start;
  std::shared_ptr<srcml_node> diff_common_end;
  std::shared_ptr<srcml_node> diff_old_start;
  std::shared_ptr<srcml_node> diff_old_end;
  std::shared_ptr<srcml_node> diff_new_start;
  std::shared_ptr<srcml_node> diff_new_end;

  std::shared_ptr<srcml_ns> diff;

  // diff attribute
  std::shared_ptr<srcml_attr> diff_type;

  std::shared_ptr<srcml_node> unit_tag;

private:

  void output_node(const srcml_node & node);
  static void update_diff_stack(std::vector<diff_set *> & open_diffs, const srcml_node * node, int operation);

public:

  srcdiff_output(srcml_archive * archive, const std::string & srcdiff_filename, const OPTION_TYPE & flags, const METHOD_TYPE & method, unsigned long number_context_lines);
  virtual ~srcdiff_output();

  virtual void initialize(int is_old, int is_new);
  virtual void start_unit(const std::string & language_string, const boost::optional<std::string> & unit_directory, const boost::optional<std::string> & unit_filename, const boost::optional<std::string> & unit_version);
  virtual void finish(int is_old, int is_new, LineDiffRange & line_diff_range);
  virtual void reset();
  virtual void close();

  virtual const std::vector<srcml_node *> & get_nodes_old() const;
  virtual const std::vector<srcml_node *> & get_nodes_new() const;
  virtual std::vector<srcml_node *> & get_nodes_old();
  virtual std::vector<srcml_node *> & get_nodes_new();
  unsigned int last_output_old() const;
  unsigned int last_output_new() const;
  virtual unsigned int & last_output_old();
  virtual unsigned int & last_output_new();
  METHOD_TYPE method() const;

  virtual void output_node(const srcml_node * node, int operation);
  virtual void output_text_as_node(const char * text, int operation);
  virtual void output_char(char character, int operation);

};

#endif
