#include <srcdiff_output.hpp>

#include <srcdiff_constants.hpp>
#include <shortest_edit_script.h>
#include <methods.hpp>

#include <list>

#include <cstring>

int move_operation = SESCOMMON;

srcdiff_output::srcdiff_output(srcml_archive * archive, const std::string & srcdiff_filename, const OPTION_TYPE & flags, const METHOD_TYPE & method,
  unsigned long number_context_lines)
 : archive(archive), colordiff(NULL), bashview(NULL), flags(flags),
   rbuf_old(std::make_shared<reader_state>(SESDELETE)), rbuf_new(std::make_shared<reader_state>(SESINSERT)), wstate(std::make_shared<writer_state>(method)),
   diff(std::make_shared<srcml_node::srcml_ns>()), diff_type(std::make_shared<srcml_node::srcml_attr>(DIFF_TYPE)) {

if(!isoption(flags, OPTION_VISUALIZE) && !isoption(flags, OPTION_BASH_VIEW))
    srcml_write_open_filename(archive, srcdiff_filename.c_str());

  // writer state
  if(isoption(flags, OPTION_VISUALIZE)) {

    std::string dir = "";
    if(srcml_archive_get_directory(archive) != NULL)
      dir = srcml_archive_get_directory(archive);

    std::string ver = "";
    if(srcml_archive_get_version(archive) != NULL)
      ver = srcml_archive_get_version(archive);

    colordiff = std::make_shared<ColorDiff>(srcdiff_filename, dir, ver, flags);

  } else if(isoption(flags, OPTION_BASH_VIEW))
    bashview = std::make_shared<bash_view>(srcdiff_filename, number_context_lines);

  wstate->filename = srcdiff_filename;

  diff->prefix = srcml_archive_get_prefix_from_uri(archive, SRCDIFF_DEFAULT_NAMESPACE_HREF.c_str());
  diff->href = SRCDIFF_DEFAULT_NAMESPACE_HREF;

  unit_tag          = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, std::string("unit"), srcml_node::srcml_ns());

  diff_common_start = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_SESCOMMON, *diff.get());
  diff_common_end   = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_SESCOMMON, *diff.get());
  diff_old_start    = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_OLD, *diff.get());
  diff_old_end      = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_OLD, *diff.get());
  diff_new_start    = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_ELEMENT, DIFF_NEW, *diff.get());
  diff_new_end      = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_END_ELEMENT, DIFF_NEW, *diff.get());

 }

 srcdiff_output::~srcdiff_output() {}

 void srcdiff_output::initialize(int is_old, int is_new) {

  diff_set * old_diff = new diff_set();
  old_diff->operation = SESCOMMON;
  rbuf_old->open_diff.push_back(old_diff);

  diff_set * new_diff = new diff_set();
  new_diff->operation = SESCOMMON;
  rbuf_new->open_diff.push_back(new_diff);

  diff_set * output_diff = new diff_set();
  output_diff->operation = SESCOMMON;
  wstate->output_diff.push_back(output_diff);

  if(!rbuf_old->nodes.empty() && !rbuf_new->nodes.empty()) {

    update_diff_stack(rbuf_old->open_diff, unit_tag, SESCOMMON);
    update_diff_stack(rbuf_new->open_diff, unit_tag, SESCOMMON);
    update_diff_stack(wstate->output_diff, unit_tag, SESCOMMON);

  } else if(rbuf_old->nodes.empty() && rbuf_new->nodes.empty()) {

    update_diff_stack(rbuf_old->open_diff, diff_common_start, SESCOMMON);
    update_diff_stack(rbuf_new->open_diff, diff_common_start, SESCOMMON);
    update_diff_stack(wstate->output_diff, diff_common_start, SESCOMMON);

    if(is_old <= -1 && is_new <= -1) {

      fprintf(stderr, "Error with files\n");

      exit(1);

    }

  } else if(rbuf_old->nodes.empty()) {

    if(!isoption(flags, OPTION_PURE)) {

      is_old = 0;
      is_new = 0;

    }

    update_diff_stack(rbuf_old->open_diff, diff_common_start, SESCOMMON);
    update_diff_stack(rbuf_new->open_diff, unit_tag, SESCOMMON);
    update_diff_stack(wstate->output_diff, unit_tag, SESCOMMON);

  } else {

    if(!isoption(flags, OPTION_PURE)) {

      is_old = 0;
      is_new = 0;

    }

    update_diff_stack(rbuf_old->open_diff, unit_tag, SESCOMMON);
    update_diff_stack(rbuf_new->open_diff, diff_common_start, SESCOMMON);
    update_diff_stack(wstate->output_diff, unit_tag, SESCOMMON);

  }


 }

 void srcdiff_output::start_unit(const std::string & language_string, const boost::optional<std::string> & unit_directory, const boost::optional<std::string> & unit_filename, const boost::optional<std::string> & unit_version) {

  wstate->unit = srcml_create_unit(archive);

  srcml_unit_set_language(wstate->unit, language_string.c_str());

  srcml_archive_get_filename(archive) ? 
    srcml_unit_set_filename(wstate->unit, srcml_archive_get_filename(archive)) : srcml_unit_set_filename(wstate->unit, unit_filename ? unit_filename->c_str() : 0);
  srcml_unit_set_directory(wstate->unit, unit_directory ? unit_directory->c_str() : 0);
  srcml_unit_set_version(wstate->unit, unit_version ? unit_version->c_str() : 0);
  /** @todo when output non-archive additional namespaces not appended, because not collected 
    However this is correct when output is to archive */
  srcml_write_start_unit(wstate->unit);

 }

 void srcdiff_output::finish(int is_old, int is_new, LineDiffRange & line_diff_range) {

  static const std::shared_ptr<srcml_node> flush = std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_TEXT, std::string("text"));
  output_node(flush, SESCOMMON);

  srcml_write_end_unit(wstate->unit);

  if(!isoption(flags, OPTION_VISUALIZE)) {

    srcml_write_unit(archive, wstate->unit);

  }

  if(isoption(flags, OPTION_VISUALIZE)) {

    if(is_old || is_new)
      colordiff->colorize(srcml_unit_get_xml(wstate->unit), line_diff_range);

  } else if(isoption(flags, OPTION_BASH_VIEW)) {

    bashview->transform(srcml_unit_get_xml(wstate->unit));
  }

  srcml_free_unit(wstate->unit);

 }

 void srcdiff_output::reset() {

  rbuf_old->clear();
  rbuf_new->clear();
  wstate->clear();

 }

void srcdiff_output::close() {

  if(!isoption(flags, OPTION_VISUALIZE) && !isoption(flags, OPTION_BASH_VIEW)) {

    srcml_close_archive(archive);

  }

}

const std::vector<std::shared_ptr<srcml_node>> & srcdiff_output::get_nodes_old() const {

  return rbuf_old->nodes;

}

const std::vector<std::shared_ptr<srcml_node>> & srcdiff_output::get_nodes_new() const {

  return rbuf_new->nodes;

}

std::vector<std::shared_ptr<srcml_node>> & srcdiff_output::get_nodes_old() {

  return rbuf_old->nodes;

}

std::vector<std::shared_ptr<srcml_node>> & srcdiff_output::get_nodes_new() {

  return rbuf_new->nodes;

}

unsigned int srcdiff_output::last_output_old() const {

  return rbuf_old->last_output;

}

unsigned int srcdiff_output::last_output_new() const {

return rbuf_new->last_output;

}

unsigned int & srcdiff_output::last_output_old() {

  return rbuf_old->last_output;

}

unsigned int & srcdiff_output::last_output_new() {

return rbuf_new->last_output;

}

METHOD_TYPE srcdiff_output::method() const {

  return wstate->method;

}

void srcdiff_output::output_node(const std::shared_ptr<srcml_node> & node, int operation) {

  /*
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, operation);
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf->output_diff.back()->operation);

    if((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT)
    fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->content);
    else
    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);
  */

  static bool delay = false;
  static int delay_operation = -2;

  // check if delaying SESDELETE/SESINSERT/SESCOMMON tag. should only stop if operation is different or not whitespace
  if(delay && (delay_operation != operation)
     && ((delay_operation == SESDELETE 
          && wstate->output_diff.back()->open_tags.back()->name == diff_old_end->name)
         || (delay_operation == SESINSERT 
             && wstate->output_diff.back()->open_tags.back()->name == diff_new_end->name)
         || (delay_operation == SESCOMMON 
             && wstate->output_diff.back()->open_tags.back()->name == diff_common_end->name))) {

    if(delay_operation == SESDELETE) {

      output_node(*diff_old_end);

      update_diff_stack(rbuf_old->open_diff, diff_old_end, SESDELETE);

      update_diff_stack(wstate->output_diff, diff_old_end, SESDELETE);

    } else if(delay_operation == SESINSERT) {

      output_node(*diff_new_end);

      update_diff_stack(rbuf_new->open_diff, diff_new_end, SESINSERT);

      update_diff_stack(wstate->output_diff, diff_new_end, SESINSERT);

    } else if(delay_operation == SESCOMMON)  {

      output_node(*diff_common_end);

      update_diff_stack(rbuf_old->open_diff, diff_common_end, SESCOMMON);
      update_diff_stack(rbuf_new->open_diff, diff_common_end, SESCOMMON);

      update_diff_stack(wstate->output_diff, diff_common_end, SESCOMMON);

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

    // check if ending a SESDELETE/SESINSERT/SESCOMMON tag. if so delay.
    if(ismethod(wstate->method, METHOD_GROUP) && operation != SESMOVE && (*node == *diff_old_end || *node == *diff_new_end || *node == *diff_common_end)) {


      delay = true;
      delay_operation = wstate->output_diff.back()->operation;
      return;

    } else {

      output_node(*node);

    }

    if(wstate->output_diff.back()->operation == SESCOMMON) {

      //fprintf(stderr, "HERE OUTPUT SESCOMMON\n");

      update_diff_stack(rbuf_old->open_diff, node, SESCOMMON);
      update_diff_stack(rbuf_new->open_diff, node, SESCOMMON);

      update_diff_stack(wstate->output_diff, node, SESCOMMON);

    } else if(wstate->output_diff.back()->operation == SESDELETE) {

      //fprintf(stderr, "HERE OUTPUT SESDELETE\n");
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

      update_diff_stack(rbuf_old->open_diff, node, SESDELETE);

      update_diff_stack(wstate->output_diff, node, SESDELETE);

    } else if(wstate->output_diff.back()->operation == SESINSERT) {

      //fprintf(stderr, "HERE OUTPUT SESINSERT\n");
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

      update_diff_stack(rbuf_new->open_diff, node, SESINSERT);

      update_diff_stack(wstate->output_diff, node, SESINSERT);

    } else if(wstate->output_diff.back()->operation == SESMOVE) {

      if(move_operation == SESDELETE)
        update_diff_stack(rbuf_old->open_diff, node, SESMOVE);
      else if(move_operation == SESINSERT)
        update_diff_stack(rbuf_new->open_diff, node, SESMOVE);

      update_diff_stack(wstate->output_diff, node, SESMOVE);

    }

    return;
  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    int current_operation = wstate->output_diff.back()->operation;
    int size = wstate->output_diff.back()->open_tags.size();

    if(size > 0 && operation != SESMOVE &&
       ((*node == *diff_old_start && current_operation == SESDELETE)
                    || (*node == *diff_new_start && current_operation == SESINSERT)
                    || (*node == *diff_common_start && current_operation == SESCOMMON))) {

      return;
    }

  }

  // output non-text node and get next node
  output_node(*node);

  if(operation == SESCOMMON) {

    //fprintf(stderr, "HERE OUTPUT SESCOMMON\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old->open_diff, node, operation);
    update_diff_stack(rbuf_new->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  }
  else if(operation == SESDELETE) {

    //fprintf(stderr, "HERE OUTPUT SESDELETE\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  } else if(operation == SESINSERT) {

    //fprintf(stderr, "HERE OUTPUT SESINSERT\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_new->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  } else if(operation == SESMOVE) {

    if(*node == *diff_old_start)
      move_operation = SESDELETE;
    else if(*node == *diff_new_start)
      move_operation = SESINSERT;

    if(move_operation == SESDELETE)
      update_diff_stack(rbuf_old->open_diff, node, operation);
    else
      update_diff_stack(rbuf_new->open_diff, node, operation);

    update_diff_stack(wstate->output_diff, node, operation);

  }

}

void srcdiff_output::update_diff_stack(std::vector<diff_set *> & open_diffs, const std::shared_ptr<srcml_node> & node, int operation) {

  // Skip empty node
  if(node->is_empty || node->is_text())
    return;

  if(open_diffs.back()->operation != operation) {

    diff_set * new_diff = new diff_set;
    new_diff->operation = operation;

    open_diffs.push_back(new_diff);
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

void srcdiff_output::output_text_as_node(const std::string & text, int operation) {

  if(text.size() == 0) return;

  output_node(std::make_shared<srcml_node>((xmlElementType)XML_READER_TYPE_TEXT, "text", boost::optional<srcml_node::srcml_ns>(), text), operation);

}


void srcdiff_output::output_char(char character, int operation) {

  static char buf[2] = { 0 };
  buf[0] = character;

  output_text_as_node(buf, operation);
}

// output current XML node in reader
void srcdiff_output::output_node(const srcml_node & node) {

  bool isemptyelement = false;

  switch (node.type) {
  case XML_READER_TYPE_ELEMENT:

    // record if this is an empty element since it will be erased by the attribute copying
    isemptyelement = node.extra & 0x1;

    // start the element
    srcml_write_start_element(wstate->unit, node.ns->prefix ? node.ns->prefix->c_str() : 0, node.name.c_str(), 0);

    // copy all the attributes
    {
      const std::list<srcml_node::srcml_attr> & attributes = node.properties;
      for(const srcml_node::srcml_attr attr : attributes) {

        srcml_write_attribute(wstate->unit, 0, attr.name.c_str(), 0, attr.value ? attr.value->c_str() : 0);

      }

    }

    // end now if this is an empty element
    if (isemptyelement) {

      srcml_write_end_element(wstate->unit);
    }

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
