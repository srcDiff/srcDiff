#include <srcdiff_output.hpp>

#include <srcdiff_constants.hpp>
#include <srcDiffUtility.hpp>
#include <shortest_edit_script.h>
#include <xmlrw.hpp>

#include <methods.hpp>

#include <string.h>

int move_operation = SESCOMMON;

srcdiff_output::srcdiff_output(const char * srcdiff_filename, METHOD_TYPE method, const char * prefix)
 : rbuf_old(std::make_shared<reader_state>(SESDELETE)), rbuf_new(std::make_shared<reader_state>(SESINSERT)), wstate(std::make_shared<writer_state>()),
  diff_common_start(std::make_shared<xNode>()), diff_common_end(std::make_shared<xNode>()),
  diff_old_start(std::make_shared<xNode>()), diff_old_end(std::make_shared<xNode>()),
  diff_new_start(std::make_shared<xNode>()), diff_new_end(std::make_shared<xNode>()),
  diff(std::make_shared<xNs>()), diff_type(std::make_shared<xAttr>()),
  unit_tag(std::make_shared<xNode>()) {

  rbuf_old->mutex = &mutex;
  rbuf_new->mutex = &mutex;

  wstate->filename = srcdiff_filename;
  wstate->method = method;

  diff->prefix = prefix;
  diff->href = SRCDIFF_DEFAULT_NAMESPACE_HREF;

  // diff attribute
  *diff_type = { 0 };
  diff_type->name = DIFF_TYPE;

  *unit_tag = { (xmlElementType)XML_READER_TYPE_ELEMENT, "unit", 0, 0, 0, 0, 0, false, false, 0, 0 };

  // diff tags
  diff_common_start->name = DIFF_SESCOMMON;
  diff_common_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_common_start->ns = diff.get();
  diff_common_start->extra = 0;

  diff_common_end->name = DIFF_SESCOMMON;
  diff_common_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_common_end->ns = diff.get();
  diff_common_end->extra = 0;

  diff_old_start->name = DIFF_OLD;
  diff_old_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_old_start->ns = diff.get();
  diff_old_start->extra = 0;

  diff_old_end->name = DIFF_OLD;
  diff_old_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_old_end->ns = diff.get();
  diff_old_end->extra = 0;

  diff_new_start->name = DIFF_NEW;
  diff_new_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_new_start->ns = diff.get();
  diff_new_start->extra = 0;

  diff_new_end->name = DIFF_NEW;
  diff_new_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_new_end->ns = diff.get();
  diff_new_end->extra = 0;

 }

 srcdiff_output::~srcdiff_output() {}

 void srcdiff_output::flush() {

  static const xNode flush = { (xmlElementType)XML_READER_TYPE_TEXT, "text", 0, "", 0, 0, 0, true, false, 0, 0 };
  output_node((xNodePtr)&flush, SESCOMMON);

 }

 void srcdiff_output::reset() {

  rbuf_old->clear();
  rbuf_new->clear();
  wstate->clear();

 }

reader_state & srcdiff_output::get_rbuf_old() {

  return *rbuf_old;

}

reader_state & srcdiff_output::get_rbuf_new() {

  return *rbuf_new;

}

writer_state & srcdiff_output::get_wstate() {

  return *wstate;

}

void srcdiff_output::output_node(const xNodePtr node, int operation) {

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
          && strcmp((const char *)wstate->output_diff.back()->open_tags.back()->name, (const char *)diff_old_end->name) == 0)
         || (delay_operation == SESINSERT 
             && strcmp((const char *)wstate->output_diff.back()->open_tags.back()->name, (const char *)diff_new_end->name) == 0)
         || (delay_operation == SESCOMMON 
             && strcmp((const char *)wstate->output_diff.back()->open_tags.back()->name, (const char *)diff_common_end->name) == 0))) {

    if(delay_operation == SESDELETE) {

      outputNode(*diff_old_end, wstate->unit);

      update_diff_stack(rbuf_old->open_diff, diff_old_end.get(), SESDELETE);

      update_diff_stack(wstate->output_diff, diff_old_end.get(), SESDELETE);

    } else if(delay_operation == SESINSERT) {

      outputNode(*diff_new_end, wstate->unit);

      update_diff_stack(rbuf_new->open_diff, diff_new_end.get(), SESINSERT);

      update_diff_stack(wstate->output_diff, diff_new_end.get(), SESINSERT);

    } else if(delay_operation == SESCOMMON)  {

      outputNode(*diff_common_end, wstate->unit);

      update_diff_stack(rbuf_old->open_diff, diff_common_end.get(), SESCOMMON);
      update_diff_stack(rbuf_new->open_diff, diff_common_end.get(), SESCOMMON);

      update_diff_stack(wstate->output_diff, diff_common_end.get(), SESCOMMON);

    }

    delay = false;
    delay_operation = -2;

  } else if(delay) {

    delay = false;
    delay_operation = -2;

  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT
       && strcmp((const char *)wstate->output_diff.back()->open_tags.back()->name, (const char *)node->name) != 0)
      return;

    // check if ending a SESDELETE/SESINSERT/SESCOMMON tag. if so delay.
    if(ismethod(wstate->method, METHOD_GROUP) && operation != SESMOVE && (*node == *diff_old_end || *node == *diff_new_end || *node == *diff_common_end)) {


      delay = true;
      delay_operation = wstate->output_diff.back()->operation;
      return;

    } else {

      outputNode(*node, wstate->unit);

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
  outputNode(*node, wstate->unit);

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

void srcdiff_output::update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation) {

  // Skip empty node
  if(node->is_empty || is_text(node))
    return;

  if(open_diffs.back()->operation != operation) {

    diff_set * new_diff = new diff_set;
    new_diff->operation = operation;

    open_diffs.push_back(new_diff);
  }

  //xNodePtr node = getRealCurrentNode(reader);
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

void srcdiff_output::output_text_as_node(const char * text, int operation) {

  if(strlen((char *)text) == 0)
    return;

  xNode node;
  node.type = (xmlElementType)XML_READER_TYPE_TEXT;
  node.name = "text";
  node.content = text;

  output_node(&node, operation);

}


void srcdiff_output::output_char(char character, int operation) {

  static char buf[2] = { 0 };
  buf[0] = character;

  output_text_as_node(buf, operation);
}

