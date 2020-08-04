#ifndef INCLUDED_SRCDIFF_OUTPUT_HPP
#define INCLUDED_SRCDIFF_OUTPUT_HPP

#include <srcml_nodes.hpp>
#include <srcdiff_options.hpp>
#include <line_diff_range.hpp>
#include <color_diff.hpp>

#include <view.hpp>
#include <unified_view.hpp>
#include <side_by_side_view.hpp>
#include <diffdoc_view.hpp>

#ifndef _MSC_BUILD
#include <srcdiff_summary.hpp>
#endif

#include <methods.hpp>
#include <srcdiff_constants.hpp>

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include <vector>
#include <memory>
#include <string>

class srcdiff_output {

public:

  struct diff_set {

    int operation;

    srcml_nodes open_tags;

  };

  // stores information on state of a single file
  class reader_state {

  public:

    reader_state(int source) 
      : stream_source(source), last_output(0) { }

    void clear() {

      last_output = 0;
      nodes.clear();

      while(!open_diff.empty()) {

        delete open_diff.back();
        open_diff.pop_back();

      }

    }

    int stream_source;
    unsigned int last_output;

    srcml_nodes nodes;

    std::vector<diff_set *> open_diff;

  };

  // stores information during xml Text Writer processing
  class writer_state {

  public:

    writer_state(const METHOD_TYPE & method) : method(method), approximate(false) {}

    void clear() {

      while(!output_diff.empty()) {

        delete output_diff.back();
        output_diff.pop_back();

      }

      approximate = false;

    }

    std::string filename;
    srcml_unit * unit;
    bool approximate;

    const METHOD_TYPE & method;

    std::vector<diff_set *> output_diff;

  };

protected:

  bool output_srcdiff;
  srcml_archive * archive;
  const OPTION_TYPE & flags;

  std::shared_ptr<reader_state> rbuf_original;
  std::shared_ptr<reader_state> rbuf_modified;
  std::shared_ptr<writer_state> wstate;

  std::shared_ptr<color_diff> colordiff;

  std::shared_ptr<view_t> view;

#ifndef _MSC_BUILD
  std::shared_ptr<srcdiff_summary> summary;
#endif

public:

  // diff nodes
  std::shared_ptr<srcml_node> diff_common_start;
  std::shared_ptr<srcml_node> diff_common_end;
  std::shared_ptr<srcml_node> diff_original_start;
  std::shared_ptr<srcml_node> diff_original_end;
  std::shared_ptr<srcml_node> diff_modified_start;
  std::shared_ptr<srcml_node> diff_modified_end;

  std::shared_ptr<srcml_node> diff_ws_start;
  std::shared_ptr<srcml_node> diff_ws_end;

  std::shared_ptr<srcml_node::srcml_ns> diff;

  std::shared_ptr<srcml_node> unit_tag;

  static bool delay;
  static int delay_operation;

private:

  static void update_diff_stack(std::vector<diff_set *> & open_diffs, const std::shared_ptr<srcml_node> & node, int operation);
  void update_diff_stacks(const std::shared_ptr<srcml_node> & node, int operation);
  void output_node(const srcml_node & node);
  void output_node_inner(const srcml_node & node);

public:

  srcdiff_output(srcml_archive * archive,
                 const std::string & srcdiff_filename,
                 const OPTION_TYPE & flags,
                 const METHOD_TYPE & method,
                 const srcdiff_options::view_options_t & view_options,
                 const boost::optional<std::string> & summary_type_str);
  virtual ~srcdiff_output();

  virtual void initialize(int is_original, int is_modified);
  virtual void start_unit(const std::string & language_string, const boost::optional<std::string> & unit_filename, const boost::optional<std::string> & unit_version);

  template<class T>
  void finish(line_diff_range<T> & line_diff_range);
  virtual void reset();
  virtual void close();


  virtual const std::string & srcdiff_filename() const;
  virtual const srcml_nodes & nodes_original() const;
  virtual const srcml_nodes & nodes_modified() const;
  virtual srcml_nodes & nodes_original();
  virtual srcml_nodes & nodes_modified();
  unsigned int last_output_original() const;
  unsigned int last_output_modified() const;
  virtual unsigned int & last_output_original();
  virtual unsigned int & last_output_modified();
  virtual int output_state() const;
  METHOD_TYPE method() const;

  void approximate(bool is_approximate);

  virtual bool is_delay_type(int operation);

  virtual void output_node(const std::shared_ptr<srcml_node> & original_node, 
                           const std::shared_ptr<srcml_node> & modified_node,
                           int operation, bool force_output = false);
  virtual void output_node(const std::shared_ptr<srcml_node> & node, int operation, bool force_output = false);
  virtual void output_text_as_node(const std::string & text, int operation);
  virtual void output_char(char character, int operation);

};

template<class T>
void srcdiff_output::finish(line_diff_range<T> & line_diff_range) {

  static const std::shared_ptr<srcml_node> flush = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_TEXT, std::string("text"));
  output_node(flush, SES_COMMON);

  if(wstate->approximate) {
    srcml_write_start_element(wstate->unit, SRCDIFF_DEFAULT_NAMESPACE_PREFIX.c_str(), "approximate", 0);
    srcml_write_end_element(wstate->unit);
  }

  srcml_write_end_unit(wstate->unit);

  if(is_option(flags, OPTION_VISUALIZE)) {

    const char * xml = srcml_unit_get_srcml(wstate->unit);
    colordiff->colorize(xml, line_diff_range);
    srcml_memory_free((char *)xml);

  } else if(is_option(flags, OPTION_UNIFIED_VIEW | OPTION_SIDE_BY_SIDE_VIEW | OPTION_DIFFDOC_VIEW)) {

    const char * xml = srcml_unit_get_srcml(wstate->unit);
    view->transform(xml, "UTF-8");
    srcml_memory_free((char *)xml);

  } else if(is_option(flags, OPTION_SUMMARY)) {

#ifndef _MSC_BUILD
    const char * xml = srcml_unit_get_srcml(wstate->unit);
    summary->summarize(xml, "UTF-8");
    srcml_memory_free((char *)xml);
#endif

  } else if(is_option(flags, OPTION_BURST)) {

    srcml_archive * srcdiff_archive = srcml_archive_clone(archive);
    srcml_archive_enable_solitary_unit(srcdiff_archive);
    srcml_archive_disable_hash(srcdiff_archive);

    std::string filename = srcml_unit_get_filename(wstate->unit);
    std::string::size_type pos;
    if((pos = filename.find('|')) != std::string::npos) {

      if(pos == 0)
        filename = filename.substr(1, std::string::npos);
      else
        filename = filename.substr(0, pos);

    }

    for(std::string::size_type pos = filename.find('/'); pos != std::string::npos; pos = filename.find('/', pos + 1))
      filename.replace(pos, 1, "_");
    filename += ".srcdiff";

    filename = wstate->filename + "/" + filename;
    srcml_archive_write_open_filename(srcdiff_archive, filename.c_str());

    srcml_archive_write_unit(srcdiff_archive, wstate->unit);
    srcml_archive_close(srcdiff_archive);
    srcml_archive_free(srcdiff_archive);

  } 

  if(output_srcdiff) {
    srcml_archive_write_unit(archive, wstate->unit);
  }

  srcml_unit_free(wstate->unit);

 }

#endif
