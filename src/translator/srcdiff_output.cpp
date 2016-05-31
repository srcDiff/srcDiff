#include <srcdiff_output.hpp>

#include <srcdiff_constants.hpp>
#include <shortest_edit_script.h>
#include <methods.hpp>

#include <list>

#include <cstring>

bool srcdiff_output::delay = false;
int srcdiff_output::delay_operation = -2;

srcdiff_output::srcdiff_output(srcml_archive * archive, 
                               const std::string & srcdiff_filename,
                               const OPTION_TYPE & flags,
                               const METHOD_TYPE & method,
                               const srcdiff_options::view_options_t & view_options,
                               const boost::optional<std::string> & summary_type_str)
 : archive(archive), flags(flags),
   rbuf_original(std::make_shared<reader_state>(SES_DELETE)), rbuf_modified(std::make_shared<reader_state>(SES_INSERT)), wstate(std::make_shared<writer_state>(method)),
   diff(std::make_shared<srcml_node::srcml_ns>()) {

  if(is_option(flags, OPTION_VISUALIZE)) {

    const std::string url = srcml_archive_get_url(archive) ? srcml_archive_get_url(archive) : "";
    const std::string version = srcml_archive_get_version(archive) ? srcml_archive_get_version(archive) : "";
    colordiff = std::make_shared<color_diff>(srcdiff_filename, url, version, flags);

  } else if(is_option(flags, OPTION_UNIFIED_VIEW)) {

     view = std::make_shared<unified_view>(srcdiff_filename,
                                           view_options.syntax_highlight,
                                           view_options.theme,
                                           is_option(flags, OPTION_IGNORE_ALL_WHITESPACE),
                                           is_option(flags, OPTION_IGNORE_WHITESPACE),
                                           is_option(flags, OPTION_IGNORE_COMMENTS),
                                           is_option(flags, OPTION_HTML_VIEW),
                                           view_options.unified_view_context);

  } else if(is_option(flags, OPTION_SIDE_BY_SIDE_VIEW)) {

     view = std::make_shared<side_by_side_view>(srcdiff_filename,
                                                view_options.syntax_highlight,
                                                view_options.theme,
                                                is_option(flags, OPTION_IGNORE_ALL_WHITESPACE),
                                                is_option(flags, OPTION_IGNORE_WHITESPACE),
                                                is_option(flags, OPTION_IGNORE_COMMENTS),
                                                is_option(flags, OPTION_HTML_VIEW),
                                                view_options.side_by_side_tab_size);

  } else if(is_option(flags, OPTION_SUMMARY)) {

#ifndef _MSC_BUILD
    summary = std::make_shared<srcdiff_summary>(srcdiff_filename, summary_type_str);
#endif
    
  } else if(!is_option(flags, OPTION_BURST)) {

      int ret_status = srcml_archive_write_open_filename(archive, srcdiff_filename.c_str(), 0);
      if(ret_status != SRCML_STATUS_OK) throw std::string("Output source '" + srcdiff_filename + "' could not be opened");

  }

  if(!is_option(flags, OPTION_BURST) || srcdiff_filename != "-")
    wstate->filename = srcdiff_filename;
  else
    wstate->filename = ".";

  diff->prefix = srcml_archive_get_prefix_from_uri(archive, SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str());
  diff->href   = SRCDIFF_DEFAULT_NAMESPACE_HREF;

  unit_tag            = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, std::string("unit"), srcml_node::srcml_ns());

  diff_common_start   = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_SES_COMMON, *diff.get());
  diff_common_end     = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_SES_COMMON, *diff.get());
  diff_original_start = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_ORIGINAL, *diff.get());
  diff_original_end   = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_ORIGINAL, *diff.get());
  diff_modified_start = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_MODIFIED, *diff.get());
  diff_modified_end   = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_MODIFIED, *diff.get());

  diff_ws_start = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_WHITESPACE, *diff.get());
  diff_ws_end   = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_WHITESPACE, *diff.get());

 }

 srcdiff_output::~srcdiff_output() {}

 void srcdiff_output::initialize(int is_original, int is_modified) {

  diff_set * original_diff = new diff_set();
  original_diff->operation = SES_COMMON;
  rbuf_original->open_diff.push_back(original_diff);

  diff_set * modified_diff = new diff_set();
  modified_diff->operation = SES_COMMON;
  rbuf_modified->open_diff.push_back(modified_diff);

  diff_set * output_diff = new diff_set();
  output_diff->operation = SES_COMMON;
  wstate->output_diff.push_back(output_diff);

  if(!rbuf_original->nodes.empty() && !rbuf_modified->nodes.empty()) {

    update_diff_stack(rbuf_original->open_diff, unit_tag, SES_COMMON);
    update_diff_stack(rbuf_modified->open_diff, unit_tag, SES_COMMON);
    update_diff_stack(wstate->output_diff, unit_tag, SES_COMMON);

  } else if(rbuf_original->nodes.empty() && rbuf_modified->nodes.empty()) {

    update_diff_stack(rbuf_original->open_diff, diff_common_start, SES_COMMON);
    update_diff_stack(rbuf_modified->open_diff, diff_common_start, SES_COMMON);
    update_diff_stack(wstate->output_diff, diff_common_start, SES_COMMON);

    if(is_original <= -1 && is_modified <= -1) {

      fprintf(stderr, "Error with files\n");

      exit(1);

    }

  } else if(rbuf_original->nodes.empty()) {

    if(!is_option(flags, OPTION_PURE)) {

      is_original = 0;
      is_modified = 0;

    }

    update_diff_stack(rbuf_original->open_diff, diff_common_start, SES_COMMON);
    update_diff_stack(rbuf_modified->open_diff, unit_tag, SES_COMMON);
    update_diff_stack(wstate->output_diff, unit_tag, SES_COMMON);

  } else {

    if(!is_option(flags, OPTION_PURE)) {

      is_original = 0;
      is_modified = 0;

    }

    update_diff_stack(rbuf_original->open_diff, unit_tag, SES_COMMON);
    update_diff_stack(rbuf_modified->open_diff, diff_common_start, SES_COMMON);
    update_diff_stack(wstate->output_diff, unit_tag, SES_COMMON);

  }

 }

 void srcdiff_output::start_unit(const std::string & language_string, const boost::optional<std::string> & unit_filename, const boost::optional<std::string> & unit_version) {

  wstate->unit = srcml_unit_create(archive);

  srcml_unit_set_language(wstate->unit, language_string.c_str());

  srcml_unit_set_filename(wstate->unit, unit_filename ? unit_filename->c_str() : 0);
  srcml_unit_set_version(wstate->unit, unit_version ? unit_version->c_str() : 0);
  /** @todo when output non-archive additional namespaces not appended, because not collected 
    However this is correct when output is to archive */
  srcml_write_start_unit(wstate->unit);

 }

 void srcdiff_output::reset() {

  rbuf_original->clear();
  rbuf_modified->clear();
  wstate->clear();

 }

void srcdiff_output::close() {

  if(!is_option(flags, OPTION_VISUALIZE | OPTION_UNIFIED_VIEW | OPTION_SUMMARY)) {

    srcml_archive_close(archive);

  }

}

const std::string & srcdiff_output::get_srcdiff_filename() const {

  return wstate->filename;


}

const srcml_nodes & srcdiff_output::get_nodes_original() const {

  return rbuf_original->nodes;

}

const srcml_nodes & srcdiff_output::get_nodes_modified() const {

  return rbuf_modified->nodes;

}

srcml_nodes & srcdiff_output::get_nodes_original() {

  return rbuf_original->nodes;

}

srcml_nodes & srcdiff_output::get_nodes_modified() {

  return rbuf_modified->nodes;

}

unsigned int srcdiff_output::last_output_original() const {

  return rbuf_original->last_output;

}

unsigned int srcdiff_output::last_output_modified() const {

return rbuf_modified->last_output;

}

unsigned int & srcdiff_output::last_output_original() {

  return rbuf_original->last_output;

}

unsigned int & srcdiff_output::last_output_modified() {

return rbuf_modified->last_output;

}

int srcdiff_output::get_output_state() const {

  return wstate->output_diff.back()->operation;

}

METHOD_TYPE srcdiff_output::method() const {

  return wstate->method;

}

void srcdiff_output::approximate(bool is_approximate) {

  wstate->approximate = is_approximate;

}


bool srcdiff_output::is_delay_type(int operation) {

  if(!delay) return false;

  return operation == delay_operation;


}

void srcdiff_output::update_diff_stack(std::vector<diff_set *> & open_diffs, const std::shared_ptr<srcml_node> & node, int operation) {

  // Skip empty node
  if(node->is_empty || node->is_text())
    return;

  if(open_diffs.back()->operation != operation) {

    diff_set * modified_diff = new diff_set;
    modified_diff->operation = operation;

    open_diffs.push_back(modified_diff);
  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    open_diffs.back()->open_tags.push_back(node);
  } else if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if(open_diffs.size() == 1 && open_diffs.back()->open_tags.size() == 1)
      return;

    open_diffs.back()->open_tags.pop_back();

  }

  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs.size());
  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs.back()->open_tags.size());
  if(open_diffs.back()->open_tags.size() == 0) {

    delete open_diffs.back();
    open_diffs.pop_back();

    //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs.size());
    //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs.back()->open_tags.size());
  }

  //fprintf(stderr, "HERE\n");

}

void srcdiff_output::update_diff_stacks(const std::shared_ptr<srcml_node> & node, int operation) {

  if(operation == SES_COMMON) {

    //fprintf(stderr, "HERE OUTPUT SES_COMMON\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_original->open_diff, node, operation);
    update_diff_stack(rbuf_modified->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  }
  else if(operation == SES_DELETE) {

    //fprintf(stderr, "HERE OUTPUT SES_DELETE\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_original->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  } else if(operation == SES_INSERT) {

    //fprintf(stderr, "HERE OUTPUT SES_INSERT\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_modified->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  }

}

void srcdiff_output::output_node(const std::shared_ptr<srcml_node> & node, int operation, bool force_output) {

  // check if delaying SES_DELETE/SES_INSERT/SES_COMMON tag. should only stop if operation is different or not whitespace
  if(delay && (delay_operation != operation)
     && ((delay_operation == SES_DELETE 
          && wstate->output_diff.back()->open_tags.back()->name == diff_original_end->name)
         || (delay_operation == SES_INSERT 
             && wstate->output_diff.back()->open_tags.back()->name == diff_modified_end->name)
         || (delay_operation == SES_COMMON 
             && wstate->output_diff.back()->open_tags.back()->name == diff_common_end->name))) {

    if(delay_operation == SES_DELETE) {

      output_node(*diff_original_end);

      update_diff_stacks(diff_original_end, delay_operation);

    } else if(delay_operation == SES_INSERT) {

      output_node(*diff_modified_end);

      update_diff_stacks(diff_modified_end, delay_operation);

    } else if(delay_operation == SES_COMMON)  {

      output_node(*diff_common_end);

      update_diff_stacks(diff_common_end, delay_operation);

    }

    delay = false;
    delay_operation = -2;

  } else if(delay) {

    delay = false;
    delay_operation = -2;

  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT && wstate->output_diff.back()->open_tags.back()->name != node->name)
      return;

    // check if ending a SES_DELETE/SES_INSERT/SES_COMMON tag. if so delay.
    if(ismethod(wstate->method, METHOD_GROUP) && !force_output && (*node == *diff_original_end || *node == *diff_modified_end || *node == *diff_common_end)) {


      delay = true;
      delay_operation = wstate->output_diff.back()->operation;
      return;

    } else {

      output_node(*node);

    }

    update_diff_stacks(node, wstate->output_diff.back()->operation);
    return;

  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    int current_operation = wstate->output_diff.back()->operation;
    int size = wstate->output_diff.back()->open_tags.size();

    if(!force_output && size > 0 &&
       (   (*node == *diff_original_start && current_operation == SES_DELETE)
        || (*node == *diff_modified_start && current_operation == SES_INSERT)
        || (*node == *diff_common_start && current_operation == SES_COMMON))) {

      return;
    }

  }

  // output non-text node and get next node
  output_node(*node);

  update_diff_stacks(node, operation);

}

void srcdiff_output::output_text_as_node(const std::string & text, int operation) {

  if(text.size() == 0) return;

  output_node(std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_TEXT, "text", srcml_node::srcml_ns(), text), operation);

}


void srcdiff_output::output_char(char character, int operation) {

  static char buf[2] = { 0 };
  buf[0] = character;

  output_text_as_node(buf, operation);
}

void srcdiff_output::output_node(const srcml_node & node) {

  static bool delay_ws_end = false;

  if(delay_ws_end) {

    delay_ws_end = false;

    if(node == *diff_ws_start) return;
    else output_node_inner(*diff_ws_end);

  }

  if(node == *diff_ws_end) {

    delay_ws_end = true;
    return;

  }

  output_node_inner(node);

}

// output current XML node in reader
void srcdiff_output::output_node_inner(const srcml_node & node) {

  bool isemptyelement = false;

  switch (node.type) {
  case XML_READER_TYPE_ELEMENT:

    // start the element
    srcml_write_start_element(wstate->unit, node.ns.prefix ? node.ns.prefix->c_str() : 0, node.name.c_str(), 0);

    // copy all the attributes
    {
      const std::list<srcml_node::srcml_attr> & attributes = node.properties;
      for(const srcml_node::srcml_attr attr : attributes) {

        srcml_write_attribute(wstate->unit, 0, attr.name.c_str(), 0, attr.value ? attr.value->c_str() : 0);

      }

    }

    // end now if this is an empty element
    if (node.is_empty)
      srcml_write_end_element(wstate->unit);

    break;

  case XML_READER_TYPE_END_ELEMENT:
    srcml_write_end_element(wstate->unit);
    break;

  case XML_READER_TYPE_COMMENT:
    //xmlTextWriterWriteComment(wstate->unit, (const xmlChar *)node.content);
    break;

  case XML_READER_TYPE_TEXT:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding
    // is handled by libxml
    srcml_write_string(wstate->unit, node.content ? node.content->c_str() : 0);
    /*for (unsigned char* p = (unsigned char*) node.content; *p != 0; ++p) {
      if (*p == '&')
        xmlTextWriterWriteRawLen(wstate->unit, BAD_CAST (unsigned char*) "&amp;", 5);
      else if (*p == '<')
        xmlTextWriterWriteRawLen(wstate->unit, BAD_CAST (unsigned char*) "&lt;", 4);
      else if (*p == '>')
        xmlTextWriterWriteRawLen(wstate->unit, BAD_CAST (unsigned char*) "&gt;", 4);
      else
        xmlTextWriterWriteRawLen(wstate->unit, BAD_CAST (unsigned char*) p, 1);
    }*/
    break;

  default:
    break;
  }

}
