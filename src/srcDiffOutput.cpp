#include "srcDiffOutput.hpp"
#include "shortest_edit_script.h"
#include "xmlrw.hpp"

#include "Methods.hpp"

#include <string.h>

// more external variables
extern xNode diff_common_start;
extern xNode diff_common_end;
extern xNode diff_old_start;
extern xNode diff_old_end;
extern xNode diff_new_start;
extern xNode diff_new_end;

int move_operation = SESCOMMON;

void output_node(reader_state & rbuf_old, reader_state & rbuf_new, const xNode * node, int operation, writer_state & wstate) {

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

  /*
    if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT
       && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)node->name) != 0)
      if((*node == diff_old_end
          || *node == diff_new_end
          || *node == diff_common_end))
        return;
  */

  // check if delaying SESDELETE/SESINSERT/SESCOMMON tag. should only stop if operation is different or not whitespace
  if(delay && (delay_operation != operation)
     && ((delay_operation == SESDELETE 
          && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)diff_old_end.name) == 0)
         || (delay_operation == SESINSERT 
             && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)diff_new_end.name) == 0)
         || (delay_operation == SESCOMMON 
             && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)diff_common_end.name) == 0))) {

    if(delay_operation == SESDELETE) {

      outputNode(diff_old_end, wstate.writer);

      update_diff_stack(rbuf_old.open_diff, &diff_old_end, SESDELETE);

      update_diff_stack(wstate.output_diff, &diff_old_end, SESDELETE);

    } else if(delay_operation == SESINSERT) {

      outputNode(diff_new_end, wstate.writer);

      update_diff_stack(rbuf_new.open_diff, &diff_new_end, SESINSERT);

      update_diff_stack(wstate.output_diff, &diff_new_end, SESINSERT);

    } else {

      outputNode(diff_common_end, wstate.writer);

      update_diff_stack(rbuf_old.open_diff, &diff_common_end, SESCOMMON);
      update_diff_stack(rbuf_new.open_diff, &diff_common_end, SESCOMMON);

      update_diff_stack(wstate.output_diff, &diff_common_end, SESCOMMON);

    }

    delay = false;
    delay_operation = -2;

  } else if(delay && (delay_operation != operation)) {

    delay = false;
    delay_operation = -2;

  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT
       && strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)node->name) != 0)
      return;

    // check if ending a SESDELETE/SESINSERT/SESCOMMON tag. if so delay.
    if(ismethod(wstate.method, METHOD_GROUP) && operation != SESMOVE && (*node == diff_old_end || *node == diff_new_end || (0 && *node == diff_common_end))) {


      delay = true;
      delay_operation = wstate.output_diff.back()->operation;
      return;

    } else
      outputNode(*node, wstate.writer);

    if(wstate.output_diff.back()->operation == SESCOMMON) {

      //fprintf(stderr, "HERE OUTPUT SESCOMMON\n");

      update_diff_stack(rbuf_old.open_diff, node, SESCOMMON);
      update_diff_stack(rbuf_new.open_diff, node, SESCOMMON);

      update_diff_stack(wstate.output_diff, node, SESCOMMON);

    } else if(wstate.output_diff.back()->operation == SESDELETE) {

      //fprintf(stderr, "HERE OUTPUT SESDELETE\n");
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

      update_diff_stack(rbuf_old.open_diff, node, SESDELETE);

      update_diff_stack(wstate.output_diff, node, SESDELETE);

    } else if(wstate.output_diff.back() == SESINSERT) {

      //fprintf(stderr, "HERE OUTPUT SESINSERT\n");
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

      update_diff_stack(rbuf_new.open_diff, node, SESINSERT);

      update_diff_stack(wstate.output_diff, node, SESINSERT);

    } else if(wstate.output_diff, node, SESMOVE) {



    }

    return;
  }

  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    int current_operation = wstate.output_diff.back()->operation;
    int size = wstate.output_diff.back()->open_tags.size();

    if(size > 0 && ((*node == diff_old_start && current_operation == SESDELETE)
                    || (*node == diff_new_start && current_operation == SESINSERT)
                    || (*node == diff_common_start && current_operation == SESCOMMON))) {

      return;
    }

  }

  // output non-text node and get next node
  outputNode(*node, wstate.writer);

  if(operation == SESCOMMON) {

    //fprintf(stderr, "HERE OUTPUT SESCOMMON\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old.open_diff, node, operation);
    update_diff_stack(rbuf_new.open_diff, node, operation);

    update_diff_stack(wstate.output_diff, node, operation);

  }
  else if(operation == SESDELETE) {

    //fprintf(stderr, "HERE OUTPUT SESDELETE\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old.open_diff, node, operation);

    update_diff_stack(wstate.output_diff, node, operation);

  } else if(operation == SESINSERT) {

    //fprintf(stderr, "HERE OUTPUT SESINSERT\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_new.open_diff, node, operation);

    update_diff_stack(wstate.output_diff, node, operation);
  } else if(operation == SESMOVE) {

    if(node_compare(node, &diff_old_start) == 0)
      move_operation == SESDELETE;
    else if(node_compare(node, &diff_new_start) == 0)
      move_operation == SESINSERT;

  }

}

void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNode * node, int operation) {

  // Skip empty node
  if(node->is_empty)
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

void output_text_as_node(reader_state & rbuf_old, reader_state & rbuf_new, char * text, int operation
                         , writer_state & wstate) {

  if(strlen((char *)text) == 0)
    return;

  xNode node;
  node.type = (xmlElementType)XML_READER_TYPE_TEXT;
  node.name = "text";
  node.content = text;

  output_node(rbuf_old, rbuf_new, &node, operation, wstate);

}


void output_char(reader_state & rbuf_old, reader_state & rbuf_new, char character, int operation
                         , writer_state & wstate) {

  static char buf[2] = { 0 };
  buf[0] = character;

  output_text_as_node(rbuf_old, rbuf_new, buf, operation, wstate);
}

