#include "srcDiffOutput.hpp"
#include "shortest_edit_script.h"
#include "xmlrw.h"

// more external variables
extern xmlNode diff_common_start;
extern xmlNode diff_common_end;
extern xmlNode diff_old_start;
extern xmlNode diff_old_end;
extern xmlNode diff_new_start;
extern xmlNode diff_new_end;

void output_node(reader_state & rbuf_old, reader_state & rbuf_new, xmlNodePtr node, int operation, writer_state & wstate) {

  /*
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, operation);
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf->output_diff.back()->operation);

    if(node->type == XML_READER_TYPE_TEXT)
    fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->content);
    else
    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);
  */

  static bool delay = false;
  static int delay_operation = -2;

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT
     && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)node->name) != 0)
    return;

  // check if delaying DELETE/INSERT/COMMON tag. should only stop if operation is different or not whitespace
  if(delay && (delay_operation != operation)) {

    if(delay_operation == DELETE) {

      outputNode(diff_old_end, wstate.writer);

      update_diff_stack(rbuf_old.open_diff, &diff_old_end, DELETE);

      update_diff_stack(wstate.output_diff, &diff_old_end, DELETE);

    } else if(delay_operation == INSERT) {

      outputNode(diff_new_end, wstate.writer);

      update_diff_stack(rbuf_new.open_diff, &diff_new_end, INSERT);

      update_diff_stack(wstate.output_diff, &diff_new_end, INSERT);

    } else {

      outputNode(diff_common_end, wstate.writer);

      update_diff_stack(rbuf_old.open_diff, &diff_common_end, COMMON);
      update_diff_stack(rbuf_new.open_diff, &diff_common_end, COMMON);

      update_diff_stack(wstate.output_diff, &diff_common_end, COMMON);

    }

    delay = false;
    delay_operation = -2;
  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT
       && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)node->name) != 0)
      return;

    // check if ending a DELETE/INSERT/COMMON tag. if so delay.
    if(*node == diff_old_end || *node == diff_new_end || *node == diff_common_end) {

      delay = true;
      delay_operation = wstate.output_diff.back()->operation;
      return;

    } else
      outputNode(*node, wstate.writer);

    if(wstate.output_diff.back()->operation == COMMON) {

      //fprintf(stderr, "HERE OUTPUT COMMON\n");

      update_diff_stack(rbuf_old.open_diff, node, COMMON);
      update_diff_stack(rbuf_new.open_diff, node, COMMON);

      update_diff_stack(wstate.output_diff, node, COMMON);

    } else if(wstate.output_diff.back()->operation == DELETE) {

      //fprintf(stderr, "HERE OUTPUT DELETE\n");
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

      update_diff_stack(rbuf_old.open_diff, node, DELETE);

      update_diff_stack(wstate.output_diff, node, DELETE);

    } else {

      //fprintf(stderr, "HERE OUTPUT INSERT\n");
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

      update_diff_stack(rbuf_new.open_diff, node, INSERT);

      update_diff_stack(wstate.output_diff, node, INSERT);
    }

    return;
  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    int current_operation = wstate.output_diff.back()->operation;
    int size = wstate.output_diff.back()->open_tags.size();

    if(size > 0 && ((*node == diff_old_start && current_operation == DELETE)
                    || (*node == diff_new_start && current_operation == INSERT)
                    || (*node == diff_common_start && current_operation == COMMON))) {

      return;
    }

  }

  // output non-text node and get next node
  outputNode(*node, wstate.writer);

  if(operation == COMMON) {

    //fprintf(stderr, "HERE OUTPUT COMMON\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old.open_diff, node, operation);
    update_diff_stack(rbuf_new.open_diff, node, operation);

    update_diff_stack(wstate.output_diff, node, operation);

  }
  else if(operation == DELETE) {

    //fprintf(stderr, "HERE OUTPUT DELETE\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old.open_diff, node, operation);

    update_diff_stack(wstate.output_diff, node, operation);

  } else {

    //fprintf(stderr, "HERE OUTPUT INSERT\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_new.open_diff, node, operation);

    update_diff_stack(wstate.output_diff, node, operation);
  }

}

void update_diff_stack(std::vector<diff_set *> & open_diffs, xmlNodePtr node, int operation) {

  // Skip empty node
  if(node->extra & 0x1)
    return;

  if(open_diffs.back()->operation != operation) {

    diff_set * new_diff = new diff_set;
    new_diff->operation = operation;

    open_diffs.push_back(new_diff);
  }

  //xmlNodePtr node = getRealCurrentNode(reader);
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
    open_diffs.pop_back();

    //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs.size());
    //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs.back()->open_tags.size());
  }

  //fprintf(stderr, "HERE\n");

}

void output_text_as_node(reader_state & rbuf_old, reader_state & rbuf_new, xmlChar * text, int operation
                         , writer_state & wstate) {

  if(strlen((char *)text) == 0)
    return;

  xmlNode node;
  node.type = (xmlElementType)XML_READER_TYPE_TEXT;
  node.name = (const xmlChar *)"text";
  node.content = text;

  output_node(rbuf_old, rbuf_new, &node, operation, wstate);

}

