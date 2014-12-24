#include <srcDiffUtility.hpp>

#include <srcdiff_diff.hpp>
#include <srcdiff_nested.hpp>
#include <srcdiff_measure.hpp>
#include <srcdiff_compare.hpp>

#include <ShortestEditScript.hpp>

#include <string.h>
#include <vector>
#include <string>
#include <xmlrw.hpp>

bool is_change(edit * edit_script) {

  return edit_script->operation == SESDELETE && edit_script->next != NULL && edit_script->next->operation == SESINSERT
    && (edit_script->offset_sequence_one + edit_script->length) == edit_script->next->offset_sequence_one;

}

bool is_white_space(const xNodePtr node) {

  // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

bool is_new_line(const xNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content[0] == '\n';

}

bool is_text(const xNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT;
}
