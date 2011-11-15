/*
  srcdiff.cpp

  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include <Options.hpp>
#include <srcMLTranslator.hpp>
#include <Language.hpp>

#include "shortest_edit_script.h"
#include "srcDiffUtility.hpp"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

// TODO:  Put this in a separate .hpp file called mingw32
#ifdef __MINGW32__
#include <string.h>
#include <io.h>

char * strndup(const char * s1, size_t n) {

  char * dest = (char *)malloc(n * sizeof(char) + 1);

  memcpy(dest, s1, n);

  dest[n] = 0;

  return dest;
}
#endif

#include "xmlrw.h"

// macros
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

#define COMMON -1

const char* XML_DECLARATION_STANDALONE = "yes";
const char* XML_VERSION = "1.0";

const char* DIFF_PREFIX = "diff:";
const char* DIFF_OLD = "diff:delete";
const char* DIFF_NEW = "diff:insert";
const char* DIFF_COMMON = "diff:common";

const char* output_encoding = "UTF-8";

const xmlChar* EDIFF_ATTRIBUTE = BAD_CAST "type";

const char* EDIFF_BEGIN = "start";
const char* EDIFF_END = "end";

xmlNode diff_common_start;
xmlNode diff_common_end;
xmlNode diff_old_start;
xmlNode diff_old_end;
xmlNode diff_new_start;
xmlNode diff_new_end;

xmlNs diff = { 0, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", 0 };

// global structures to hold read in nodes
std::vector<xmlNode *> nodes_old;
std::vector<xmlNode *> nodes_new;

// converts source code to srcML
xmlBuffer * translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

struct open_diff {

  int operation;

  std::vector<xmlNodePtr> open_tags;

};

// stores information on state of a single file
struct reader_state {

  int stream_source;
  int last_output;

  // just a pointer not on stack
  std::vector<struct open_diff *> open_diff;

};

// stores information during xml Text Writer processing
struct writer_state {

  xmlTextWriterPtr writer;
  std::vector<struct open_diff *> output_diff;

};

const char * block_types[] = { "block", "if", "while", "for", "function", 0 };

const char * nest_types[] = { "block", "expr_stmt", "decl_stmt", 0 };

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// create sets of nodes
std::vector<std::vector<int> *> create_node_set(std::vector<xmlNodePtr> * nodes, int start, int end);

// collect the differnces
void collect_difference(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader);

// output a single difference DELETE or INSERT
void output_single(struct reader_state & rbuf_old, struct reader_state & rbuf_new, struct edit * edit, struct writer_state & wstate);

// output file level info
void output_diffs(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate);

void output_comment_paragraph(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate);

void output_comment_line(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate);

void output_comment_word(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate);

void output_recursive(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , unsigned int start_old
                      , struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , unsigned int start_new
                      , struct writer_state & wstate);

void output_change(struct reader_state & rbuf_old, unsigned int end_old, struct reader_state & rbuf_new, unsigned int end_new
                   , struct writer_state & wstate);

void output_change_white_space(struct reader_state & rbuf_old, unsigned int end_old
                               , struct reader_state & rbuf_new, unsigned int end_new
                               , struct writer_state & wstate);

void compare_many2many(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , struct edit * edit_script, struct writer_state & wstate);

void output_node(struct reader_state & rbuf_old, struct reader_state & rbuf_new, xmlNodePtr node, int operation, struct writer_state & wstate);

void update_diff_stack(std::vector<struct open_diff *> & open_diffs, xmlNodePtr node, int operation);

void markup_whitespace(struct reader_state & rbuf_old, unsigned int end_old, struct reader_state & rbuf_new, unsigned int end_new, struct writer_state & wstate);

void output_char(char character, struct writer_state & wstate);

bool is_nestable(std::vector<int> * structure_one, std::vector<xmlNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xmlNodePtr> & nodes_two);

bool has_interal_block(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes);

void output_nested(struct reader_state rbuf_old, std::vector<int> * structure_old
                   , struct reader_state rbuf_new ,std::vector<int> * structure_new
                   , int operation, struct writer_state wstate);

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    fprintf(stderr, "Usage: srcdiff oldFile newFile dir\n");
    return 1;
  }

  const char * srcdiff_file;
  srcdiff_file = "-";

  // TODO: mabe put this in a function
  diff_common_start.name = (xmlChar *) DIFF_COMMON;
  diff_common_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_common_start.extra = 0;

  diff_common_end.name = (xmlChar *) DIFF_COMMON;
  diff_common_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_common_end.extra = 0;

  diff_old_start.name = (xmlChar *) DIFF_OLD;
  diff_old_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_old_start.extra = 0;

  diff_old_end.name = (xmlChar *) DIFF_OLD;
  diff_old_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_old_end.extra = 0;

  diff_new_start.name = (xmlChar *) DIFF_NEW;
  diff_new_start.type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_new_start.extra = 0;

  diff_new_end.name = (xmlChar *) DIFF_NEW;
  diff_new_end.type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_new_end.extra = 0;

  // TODO: Error handling?
  // translate file one
  xmlBuffer * output_file_one = translate_to_srcML(argv[1], 0, argv[3]);

  // translate file two
  xmlBuffer * output_file_two = translate_to_srcML(argv[2], 0, argv[3]);

  /*
    Create xmlreaders and the xmlwriter
  */

  xmlTextReaderPtr reader_old = NULL;
  xmlTextReaderPtr reader_new = NULL;

  xmlTextWriterPtr writer = NULL;

  // create the reader for the old file
  reader_old = xmlReaderForMemory((const char*) xmlBufferContent(output_file_one), output_file_one->use, 0, 0, 0);
  if (reader_old == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", argv[1]);

    exit(1);
  }

  // create the reader for the new file
  reader_new = xmlReaderForMemory((const char*) xmlBufferContent(output_file_two), output_file_two->use, 0, 0, 0);
  if (reader_new == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", argv[2]);

    if(reader_old)
      xmlFreeTextReader(reader_old);

    exit(1);
  }

  // create the writer
  writer = xmlNewTextWriterFilename(srcdiff_file, 0);
  if (writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", srcdiff_file);

    if(reader_old)
      xmlFreeTextReader(reader_old);

    if(reader_new)
      xmlFreeTextReader(reader_new);

    exit(1);
  }

  // issue the xml declaration
  xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);

  // set up writer state
  std::vector<struct open_diff *> output_diff;
  struct open_diff * new_diff = new struct open_diff;
  new_diff->operation = COMMON;
  output_diff.push_back(new_diff);

  struct writer_state wstate = { 0 };
  wstate.writer = writer;
  wstate.output_diff = output_diff;


  // Set up delete reader state
  struct reader_state rbuf_old = { 0 };
  rbuf_old.stream_source = DELETE;
  std::vector<struct open_diff *> open_diff_old;
  rbuf_old.open_diff = open_diff_old;

  new_diff = new struct open_diff;
  new_diff->operation = COMMON;
  rbuf_old.open_diff.push_back(new_diff);

  // read to unit
  xmlTextReaderRead(reader_old);

  // Set up insert reader state
  struct reader_state rbuf_new = { 0 };
  rbuf_new.stream_source = INSERT;
  std::vector<struct open_diff *> open_diff_new;
  rbuf_new.open_diff = open_diff_new;

  new_diff = new struct open_diff;
  new_diff->operation = COMMON;
  rbuf_new.open_diff.push_back(new_diff);

  // read to unit
  xmlTextReaderRead(reader_new);

  // create srcdiff unit
  xmlNodePtr unit = create_srcdiff_unit(reader_old, reader_new);

  // output srcdiff unit
  output_node(rbuf_old, rbuf_new, unit, COMMON, wstate);

  // Read past unit tag open
  int is_old = xmlTextReaderRead(reader_old);
  int is_new = xmlTextReaderRead(reader_new);

  // collect if non empty files
  if(is_old)
    collect_difference(&nodes_old, reader_old);

  // free the buffer
  xmlBufferFree(output_file_one);

  // collect if non empty files
  if(is_new)
    collect_difference(&nodes_new, reader_new);

  // free the buffer
  xmlBufferFree(output_file_two);

  // group nodes
  std::vector<std::vector<int> *> node_set_old = create_node_set(&nodes_old, 0, nodes_old.size());
  std::vector<std::vector<int> *> node_set_new = create_node_set(&nodes_new, 0, nodes_new.size());

  // run on file level
  output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

  // output srcdiff unit
  outputNode(*getRealCurrentNode(reader_old), wstate.writer);

  // cleanup everything
  if(reader_old)
    xmlFreeTextReader(reader_old);

  if(reader_new)
    xmlFreeTextReader(reader_new);

  if(writer) {

    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
  }

  return 0;
}

// converts source code to srcML
xmlBuffer* translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir) {

  // register default language extensions
  Language::register_standard_file_extensions();

  // get language from file extension
  int language = Language::getLanguageFromFilename(source_file);

  // select basic options
  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML  | OPTION_LITERAL | OPTION_OPERATOR | OPTION_MODIFIER;

  xmlBuffer* output_buffer = xmlBufferCreate();

  // create translator object
  srcMLTranslator translator(language, output_buffer, options);

  // set input file (must be done)
  translator.setInput(source_file);

  // translate file
  translator.translate(NULL, dir, NULL, NULL, language);

  // close the input file
  translator.close();

  return output_buffer;
}

// collect the differnces
void collect_difference(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader) {

  int not_done = 1;
  while(not_done) {

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      const char * characters = (const char *)xmlTextReaderConstValue(reader);

      // cycle through characters
      for (; (*characters) != 0;) {

        const char * characters_start = characters;

        xmlNode * text = new xmlNode;
        text->type = (xmlElementType)XML_READER_TYPE_TEXT;
        text->name = (const xmlChar *)"text";

        // separate new line
        if(*characters == '\n') {

          text->content = (xmlChar *)"\n";
          ++characters;
        }

        // separate non-new line whitespace
        else if(isspace(*characters)) {

          while((*characters) != 0 && *characters != '\n' && isspace(*characters))
            ++characters;

          const char * content = strndup((const char *)characters_start, characters  - characters_start);
          text->content = (xmlChar *)content;

        }

        // separate non whitespace
        else {

          while((*characters) != 0 && !isspace(*characters))
            ++characters;

          const char * content = strndup((const char *)characters_start, characters  - characters_start);
          text->content = (xmlChar *)content;

        }

        nodes->push_back(text);

      }
    }
    else {

      xmlNodePtr node = getRealCurrentNode(reader);

      if(strcmp((const char *)node->name, "unit") == 0)
        return;

      // save non-text node and get next node
      nodes->push_back(node);

    }

    not_done = xmlTextReaderRead(reader);

  }

}

// check if node is a indivisable group of three (atomic)
bool is_atomic_srcml(std::vector<xmlNodePtr> * nodes, unsigned start) {

  if((start + 2) >= nodes->size())
    return false;

  if((xmlReaderTypes)nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if((xmlReaderTypes)nodes->at(start + 2)->type != XML_READER_TYPE_END_ELEMENT)
    return false;

  if(strcmp((const char *)nodes->at(start)->name, (const char *)nodes->at(start + 2)->name) != 0)
    return false;

  if(strcmp((const char *)nodes->at(start)->name, "name") == 0)
    return true;

  if(strcmp((const char *)nodes->at(start)->name, "operator") == 0)
    return true;

  if(strcmp((const char *)nodes->at(start)->name, "literal") == 0)
    return true;

  if(strcmp((const char *)nodes->at(start)->name, "modifier") == 0)
    return true;

  return false;
}

// collect an entire tag from open tag to closing tag
void collect_entire_tag(std::vector<xmlNodePtr> * nodes, std::vector<int> * node_set, int * start) {

  //const char * open_node = (const char *)nodes->at(*start)->name;

  node_set->push_back(*start);

  if(nodes->at(*start)->extra & 0x1)
    return;

  ++(*start);

  // track open tags because could have same type nested
  int is_open = 1;
  for(; is_open; ++(*start)) {

    // skip whitespace
    if(!is_white_space(nodes->at(*start))) {

      node_set->push_back(*start);

      // opening tags
      if((xmlReaderTypes)nodes->at(*start)->type == XML_READER_TYPE_ELEMENT
         && !(nodes->at(*start)->extra & 0x1))
        ++is_open;

      // closing tags
      else if((xmlReaderTypes)nodes->at(*start)->type == XML_READER_TYPE_END_ELEMENT)
        --is_open;

    }

  }

  --(*start);
}

// create the node sets for shortest edit script
std::vector<std::vector<int> *> create_node_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  std::vector<std::vector<int> *> node_sets;

  // runs on a subset of base array
  for(int i = start; i < end; ++i) {

    // skip whitespace
    if(!is_white_space(nodes->at(i))) {

      std::vector <int> * node_set = new std::vector <int>;

      // text is separate node if not surrounded by a tag in range
      if((xmlReaderTypes)nodes->at(i)->type == XML_READER_TYPE_TEXT) {
        //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->content);
        node_set->push_back(i);

        // a base case of sorts
      } else if(is_atomic_srcml(nodes, i)) {

        node_set->push_back(i);
        node_set->push_back(i + 1);
        node_set->push_back(i + 2);

        i += 2;

        // open tags handle here
      } else if((xmlReaderTypes)nodes->at(i)->type == XML_READER_TYPE_ELEMENT) {

        collect_entire_tag(nodes, node_set, &i);

      } else {

        // could be a closing tag, but then something should be wrong.
        // TODO: remove this and make sure it works
        node_set->push_back(i);
      }

      node_sets.push_back(node_set);

    }

  }

  return node_sets;

}

/*

  Output common elements.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after has no newline,
  it is included and the following nodes are included if they have a new line.

*/
void output_common(struct reader_state & rbuf_old, unsigned int end_old
                   , struct reader_state & rbuf_new, unsigned int end_new
                   , struct writer_state & wstate) {

  unsigned int oend = end_old;
  unsigned int nend = end_new;

  // advance whitespace after targeted end
  if(oend < nodes_old.size() && is_white_space(nodes_old.at(oend)))
    ++oend;

  if( nend < nodes_new.size() && is_white_space(nodes_new.at(nend)))
    ++nend;

  for(; oend < nodes_old.size() && is_new_line(nodes_old.at(oend)); ++oend)
    ;
  for(; nend < nodes_new.size() && is_new_line(nodes_new.at(nend)); ++nend)
    ;

  // output common tag if needed
  if(rbuf_old.open_diff.back()->operation != COMMON)
    output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);

  // output common nodes
  markup_whitespace(rbuf_old, oend, rbuf_new, nend, wstate);

  // output common tag if needed
  output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

}

/*

  Outputs diff on each level.  First, Common areas as well as inserts and deletes
  are output directly.  One-to-one matches result in recursion on syntax match and
  direct output otherwise.  Many-to-many decides to treat parts or all of the
  change as multiple one-one or deletions followed by insert.

  Whitespace is dis-regarded during the entire process, and is incorporated on
  output before and after changes/common sections.

*/
void output_diffs(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate) {

  //fprintf(stderr, "HERE_DOUBLE\n");

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // 1-1
      if(edits->length == edit_next->length && edits->length == 1
         && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
             || node_sets_old->at(edits->offset_sequence_one)->size() > 1)) {

        // syntax match
        if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))
                        , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))) == 0
           && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type != XML_READER_TYPE_TEXT) {

          output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one
                           , rbuf_new, node_sets_new, edit_next->offset_sequence_two, wstate);

        } else {


          if(is_nestable(node_sets_old->at(edits->offset_sequence_one)
                         , nodes_old, node_sets_new->at(edit_next->offset_sequence_two), nodes_new)) {

            //output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)
            //              , INSERT, wstate);
          // syntax mismatch
          output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                                    , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, wstate);

          } else if(is_nestable(node_sets_new->at(edit_next->offset_sequence_two)
                                , nodes_new, node_sets_old->at(edits->offset_sequence_one), nodes_old)) {

            output_nested(rbuf_old, node_sets_old->at(edits->offset_sequence_one), rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)
                          , DELETE, wstate);

          } else {

          // syntax mismatch
          output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                                    , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, wstate);
          }

        }

      } else {

        // many to many handling
        compare_many2many(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edits, wstate);

      }

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_change_white_space(rbuf_old, 0
                                  , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, wstate);


      // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, 0, wstate);

      // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

        break;
      }
    }

  }

  // determine ending position to output
  diff_end_old = rbuf_old.last_output;
  diff_end_new = rbuf_new.last_output;
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

  free_shortest_edit_script(edit_script);

}

/*
  Collect paragraphs
*/
std::vector<std::vector<int> *> create_comment_paragraph_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  // collect all the paragraphs separated by double newlines
  std::vector<std::vector<int> *> node_sets;
  for(int i = start; i < end; ++i) {

    // move past any starting newlines
    for(; is_new_line(nodes->at(i)); ++i)
      ;

    // collect the nodes in the paragraph
    std::vector <int> * node_set = new std::vector <int>;

    int newlines = 0;
    for(; i < end; ++i) {

      if(is_new_line(nodes->at(i)))
        ++newlines;

      if(newlines > 1)
        break;

      if(!is_white_space(nodes->at(i)))
        node_set->push_back(i);
    }

    node_sets.push_back(node_set);

  }

  return node_sets;

}

// collect lines
std::vector<std::vector<int> *> create_comment_line_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  std::vector<std::vector<int> *> node_sets;

  for(int i = start; i < end; ++i) {

    std::vector<int> * node_set = new std::vector <int>;

    for(; i < end; ++i) {

      // stop the node set at the newline
      if(is_new_line(nodes->at(i)))
        break;

      // only collect non-whitespace nodes
      if(!is_white_space(nodes->at(i)))
        node_set->push_back(i);
    }

    node_sets.push_back(node_set);

  }

  return node_sets;

}

/*

  Once a comment change is encountered, they are first matched based
  on paragraphs (separated by two new lines), then as needed, they are processes by line
  , and then by word using shortest edit script. Whitespace is included after/before changes

*/
void output_comment_paragraph(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate) {

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      // 1-1
      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> next_node_set_old
          = create_comment_line_set(&nodes_old, node_sets_old->at(edits->offset_sequence_one)->at(0)
                                    , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        std::vector<std::vector<int> *> next_node_set_new
          = create_comment_line_set(&nodes_new, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                                    , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        // compare as lines
        output_comment_line(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

      } else {

        // many to many
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);
      }

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_change_white_space(rbuf_old, 0
                                  , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, wstate);
        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, 0, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

        break;
      }

    }

  }

  // determine ending position to output
  diff_end_old = rbuf_old.last_output;
  diff_end_new = rbuf_new.last_output;
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

  free_shortest_edit_script(edit_script);

}

/*

  Breaks down paragraphs and runs on a line bases.
  Whitespace is included after/before changes

*/
void output_comment_line(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate) {

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  struct edit * edits = edit_script;

  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      // 1-1
      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> next_node_set_old
          = create_node_set(&nodes_old, node_sets_old->at(edits->offset_sequence_one)->at(0)
                            , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        std::vector<std::vector<int> *> next_node_set_new
          = create_node_set(&nodes_new, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                            , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        // compare on word basis
        output_comment_word(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

      } else

        // many to many
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_change_white_space(rbuf_old, 0
                                  , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                                  , rbuf_new, 0, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

        break;
      }

    }
  }

  // determine ending position to output
  diff_end_old = rbuf_old.last_output;
  diff_end_new = rbuf_new.last_output;
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);


}

/*

  Outputs differences in a comment on a word bases.

  Whitespace is not included with changes, but marked up
  where in common.

*/
void output_comment_word(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new, struct writer_state & wstate) {

  //fprintf(stderr, "HERE_DOUBLE\n");

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old.last_output;
  int diff_end_new = rbuf_new.last_output;

  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // determine ending position to output
    diff_end_old = rbuf_old.last_output;
    diff_end_new = rbuf_new.last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

    // output area in common
    output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      // TODO: simplify unless plan to handle many to many different // 1-1
      if(edits->length == edit_next->length && edits->length == 1
         && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
             || node_sets_old->at(edits->offset_sequence_one)->size() > 1)) {

        output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, wstate);

      } else {

        // many to many
        output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, wstate);

      }

      // update for common
      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;

    } else {

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:

        //fprintf(stderr, "HERE\n");
        output_change(rbuf_old, 0
                      , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + 1;
        last_diff_new = edits->offset_sequence_two + edits->length;

        break;

      case DELETE:

        //fprintf(stderr, "HERE\n");
        output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                      , rbuf_new, 0, wstate);

        // update for common
        last_diff_old = edits->offset_sequence_one + edits->length;
        last_diff_new = edits->offset_sequence_two + 1;

        break;
      }

    }
  }

  // determine ending position to output
  diff_end_old = rbuf_old.last_output;
  diff_end_new = rbuf_new.last_output;
  if(last_diff_old < (signed)node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }

  // output area in common
  output_common(rbuf_old, diff_end_old, rbuf_new, diff_end_new, wstate);

  free_shortest_edit_script(edit_script);

}



void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new) {

  // get units from source code
  xmlNodePtr unit = getRealCurrentNode(reader_old);
  //xmlNodePtr unit_new = getRealCurrentNode(reader_new);

  // add diff namespace
  addNamespace(&unit->nsDef, &diff);

  return unit;
}

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns) {

  xmlNsPtr namespaces = *nsDef;

  if(namespaces) {

    for(; namespaces->next; namespaces = namespaces->next)
      ;

    namespaces->next = ns;
  }
  else
    *nsDef = ns;

}

void update_diff_stack(std::vector<struct open_diff *> & open_diffs, xmlNodePtr node, int operation) {

  // Skip empty node
  if(node->extra & 0x1)
    return;

  if(open_diffs.back()->operation != operation) {

    struct open_diff * new_diff = new struct open_diff;
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

void output_node(struct reader_state & rbuf_old, struct reader_state & rbuf_new, xmlNodePtr node, int operation, struct writer_state & wstate) {

  /*
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, operation);
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf->output_diff.back()->operation);

    if(node->type == XML_READER_TYPE_TEXT)
    fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->content);
    else
    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);
  */

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if(strcmp((const char *)wstate.output_diff.back()->open_tags.back()->name, (const char *)node->name) != 0)
      return;

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

struct offset_pair {

  int old_offset;
  int old_length;
  int new_offset;
  int new_length;
  int similarity;
  struct offset_pair * next;
};

#define MIN -1

int compute_similarity(std::vector<int> * node_set_old, std::vector<int> * node_set_new) {

  //unsigned int length = node_set_new->size();

  if(node_set_syntax_compare(node_set_old, node_set_new) == 0)
    return MIN;


  unsigned int leftptr;
  for(leftptr = 0; leftptr < node_set_old->size() && leftptr < node_set_new->size() && node_compare(nodes_old.at(node_set_old->at(leftptr)), nodes_new.at(node_set_new->at(leftptr))) == 0; ++leftptr)
    ;

  unsigned int rightptr;
  for(rightptr = 1; rightptr <= node_set_old->size() && rightptr <= node_set_new->size()
        && node_compare(nodes_old.at(node_set_old->at(node_set_old->size() - rightptr)),
                        nodes_new.at(node_set_new->at(node_set_new->size() - rightptr))) == 0; ++rightptr)
    ;

  int old_diff = ((int)node_set_old->size() - rightptr) - leftptr;
  int new_diff = ((int)node_set_new->size() - rightptr) - leftptr;

  int value = ((old_diff > new_diff) ? old_diff : new_diff);

  if(value < 0)
    value = 0;

  return 0;
}

void match_differences(std::vector<std::vector<int> *> * node_sets_old
                       , std::vector<std::vector<int> *> * node_sets_new
                       , struct edit * edit_script, struct offset_pair ** matches) {

  struct edit * edits = edit_script;
  struct edit * edit_next = edit_script->next;

  for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

    // TODO: set to first or positive infinity or MAX_INT or whatever it is called
    int min_similarity = 65535;
    for(int pos = old_pos; pos < edits->length; ++pos) {

      int similarity = 0;
      if((similarity = compute_similarity(node_sets_old->at(edits->offset_sequence_one + pos)
                                          , node_sets_new->at(edit_next->offset_sequence_two + new_pos))) < min_similarity) {


        old_pos = pos;
        min_similarity = similarity;

      }

    }

    struct offset_pair * match = new struct offset_pair;
    match->old_offset = old_pos;
    match->new_offset = new_pos;
    match->similarity = min_similarity;
    match->next = NULL;

    if(new_pos == 0)
      *matches = match;
    else
      (*matches)->next = match;

  }

}

void output_unmatched(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old, int end_old
                      , struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new, int end_new
                      , struct writer_state & wstate) {

  unsigned int finish_old = rbuf_old.last_output;
  unsigned int finish_new = rbuf_new.last_output;

  if(start_old <= end_old && start_old >= 0 && end_old < (signed)node_sets_old->size()) {

    finish_old = node_sets_old->at(end_old)->back() + 1;
  }

  if(start_new <= end_new && start_new >= 0 && end_new < (signed)node_sets_new->size()) {

    finish_new = node_sets_new->at(end_new)->back() + 1;
  }

  output_change_white_space(rbuf_old, finish_old, rbuf_new, finish_new, wstate);

}

void compare_many2many(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , struct edit * edit_script, struct writer_state & wstate) {

  struct edit * edits = edit_script;
  struct edit * edit_next = edit_script->next;

  struct offset_pair * matches;

  match_differences(node_sets_old, node_sets_new, edit_script, &matches);

  int last_old = 0;
  int last_new = 0;

  for(; matches; matches = matches->next) {

    // output diffs until match
    output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                     edits->offset_sequence_one + matches->old_offset - 1,
                     rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                     , edit_next->offset_sequence_two + matches->new_offset - 1, wstate);

    // correct could only be whitespace
    if(matches->similarity == MIN) {


      if(rbuf_old.open_diff.back()->operation != COMMON)
        output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);


      markup_whitespace(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1

                        , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1
                        , wstate);

      output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

    } else if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))
                           , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->at(0))) == 0
              && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))->type != XML_READER_TYPE_TEXT) {

      output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset
                       , rbuf_new, node_sets_new, edit_next->offset_sequence_two + matches->new_offset, wstate);

      output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

    } else {

      output_change_white_space(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1,
                                rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1
                                , wstate);

    }

    last_old = matches->old_offset + 1;
    last_new = matches->new_offset + 1;

  }

  // output diffs until match
  output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                   edits->offset_sequence_one + edits->length - 1,
                   rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                   , edit_next->offset_sequence_two + edit_next->length - 1, wstate);


}

void output_recursive(struct reader_state & rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , unsigned int start_old
                      , struct reader_state & rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , unsigned int start_new
                      , struct writer_state & wstate) {

  if(rbuf_old.open_diff.back()->operation != COMMON)
    output_node(rbuf_old, rbuf_new, &diff_common_start, COMMON, wstate);


  markup_whitespace(rbuf_old, node_sets_old->at(start_old)->at(0), rbuf_new, node_sets_new->at(start_new)->at(0), wstate);

  output_node(rbuf_old, rbuf_new, nodes_old.at(node_sets_old->at(start_old)->at(0)), COMMON, wstate);

  ++rbuf_old.last_output;
  ++rbuf_new.last_output;

  // compare subset of nodes

  if(strcmp((const char *)nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_comment_paragraph_set(&nodes_old, node_sets_old->at(start_old)->at(1)
                                     , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    std::vector<std::vector<int> *> next_node_set_new
      = create_comment_paragraph_set(&nodes_new, node_sets_new->at(start_new)->at(1)
                                     , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    output_comment_paragraph(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

  }
  else {

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(&nodes_old, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->back());

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(&nodes_new, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->back());

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);


  }

  markup_whitespace(rbuf_old, node_sets_old->at(start_old)->back() + 1, rbuf_new, node_sets_new->at(start_new)->back() + 1, wstate);

  /*
    output_node(rbuf_old, rbuf_new,
    nodes_old.at(node_sets_old->at(start_old)->
    at(node_sets_old->at(start_old)->size() - 1))
    , COMMON, wstate);

    ++rbuf_old.last_output;
    ++rbuf_new.last_output;
  */


  output_node(rbuf_old, rbuf_new, &diff_common_end, COMMON, wstate);

}

/*

  Output of same syntactical entities, but possible whitespace differences.

*/
void markup_whitespace(struct reader_state & rbuf_old, unsigned int end_old, struct reader_state & rbuf_new, unsigned int end_new, struct writer_state & wstate) {

  unsigned int begin_old = rbuf_old.last_output;
  unsigned int begin_new = rbuf_new.last_output;

  unsigned int oend = end_old;
  unsigned int nend = end_new;

  unsigned int i, j;
  for(i = begin_old, j = begin_new; i < oend && j < nend; ++i, ++j) {

    if(node_compare(nodes_old.at(i), nodes_new.at(j)) == 0)

      output_node(rbuf_old, rbuf_new, nodes_old.at(i), COMMON, wstate);

    else if(is_white_space(nodes_old.at(i)) && is_white_space(nodes_new.at(j))) {

      xmlChar * content_old = nodes_old.at(i)->content;
      xmlChar * content_new = nodes_new.at(j)->content;

      int size_old = strlen((const char *)content_old);
      int size_new = strlen((const char *)content_new);

      int ostart = 0;
      int nstart = 0;

      for(; ostart < size_old && nstart < size_new && content_old[ostart] == content_new[nstart]; ++ostart, ++nstart)
        ;

      xmlTextWriterWriteRawLen(wstate.writer, content_old, ostart);

      if(ostart < size_old) {


        if(rbuf_old.open_diff.back()->operation != DELETE)
          output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

        // output diff tag
        xmlTextWriterWriteRawLen(wstate.writer, content_old + ostart, size_old - ostart);

        // output diff tag
        output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

      }

      if(nstart < size_new) {

        if(rbuf_old.open_diff.back()->operation != INSERT)
          output_node(rbuf_new, rbuf_new, &diff_new_start, INSERT, wstate);
        // output diff tag

        xmlTextWriterWriteRawLen(wstate.writer, content_new + nstart, size_new - nstart);

        // output diff tag
        output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

      }

      // whitespace change
    } else if(is_white_space(nodes_old.at(i))) {

      if(rbuf_old.open_diff.back()->operation != DELETE)
        output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);
      // whitespace delete
      // output diff tag

      output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

      // output diff tag
      output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

      --j;

    } else if(is_white_space(nodes_new.at(j))) {

      if(rbuf_old.open_diff.back()->operation != INSERT)
        output_node(rbuf_new, rbuf_new, &diff_new_start, INSERT, wstate);
      //whitespace insert
      // output diff tag

      output_node(rbuf_old, rbuf_new, nodes_new.at(j), INSERT, wstate);

      // output diff tag
      output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

      --i;

    } else if(is_text(nodes_old.at(i)) && is_text(nodes_new.at(j))) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_old = "";
      for(; i < end_old && is_text(nodes_old.at(i)); ++i)
        text_old += (const char *)nodes_old.at(i)->content;

      std::string text_new = "";
      for(; j < end_new && is_text(nodes_new.at(j)); ++j)
        text_new += (const char *)nodes_new.at(j)->content;

      --i;
      --j;

      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, text_old.c_str());
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, text_new.c_str());

      for(unsigned int opos = 0, npos = 0; opos < text_old.size() && npos < text_new.size();) {

        if(text_old[opos] == text_new[npos]) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
          output_char(text_old[opos], wstate);

          ++opos;
          ++npos;

        } else {

          if(isspace(text_old[opos]) || isspace(text_new[npos])) {

            if(isspace(text_old[opos])) {

              if(rbuf_old.open_diff.back()->operation != DELETE)
                output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

              for(; opos < text_old.size() && isspace(text_old[opos]); ++opos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
                output_char(text_old[opos], wstate);
              }

              // output diff tag
              output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

            }

            if(isspace(text_new[npos])) {

              if(rbuf_old.open_diff.back()->operation != INSERT)
                output_node(rbuf_new, rbuf_new, &diff_new_start, INSERT, wstate);

              for(; npos < text_new.size() && isspace(text_new[npos]); ++npos) {

                //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
                output_char(text_new[npos], wstate);
              }

              // output diff tag
              output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

            }

          }

        }

      }

    } else {

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes_old.at(i)->name);
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes_new.at(i)->name);

      // should never reach this state  This usually occurs when the two lines are not actually the same i.e. more than just whitespace
      fprintf(stderr, "ERROR");
      exit(1);
    }

  }

  // output leftover nodes
  if(i < oend) {

    if(rbuf_old.open_diff.back()->operation != DELETE)
      output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);
    // whitespace delete
    // output diff tag

    for( ; i < oend; ++i)
      output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

  } else if(j < nend) {

    if(rbuf_new.open_diff.back()->operation != INSERT)
      output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);
    // whitespace delete
    // output diff tag

    for( ; j < nend; ++j)
      output_node(rbuf_old, rbuf_new, nodes_new.at(j), INSERT, wstate);

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_new_end, DELETE, wstate);

  }

  rbuf_old.last_output = oend;
  rbuf_new.last_output = nend;

}

/*

  Adds whitespace to a change. Then outputs the change.

  All preceeding unused whitespace must be included, and all whitespace
  with a newline afterwards.  Currently, if the first after is not a newline,
  it is included and the following nodes are included if they have a new line.

*/
void output_change_white_space(struct reader_state & rbuf_old, unsigned int end_old
                               , struct reader_state & rbuf_new, unsigned int end_new
                               , struct writer_state & wstate) {

  unsigned int oend = end_old;
  unsigned int nend = end_new;

  if(oend < nodes_old.size() && is_white_space(nodes_old.at(oend)))
    ++oend;

  if( nend < nodes_new.size() && is_white_space(nodes_new.at(nend)))
    ++nend;

  for(; oend < nodes_old.size() && is_new_line(nodes_old.at(oend)); ++oend)
    ;

  for(; nend < nodes_new.size() && is_new_line(nodes_new.at(nend)); ++nend)
    ;

  output_change(rbuf_old, oend, rbuf_new, nend, wstate);


}

/*

  Outputs a syntactical diff. Beginning whitespace is narrowed and all
  whitespace even if it could be matches is treated as different.

*/
void output_change(struct reader_state & rbuf_old, unsigned int end_old
                   , struct reader_state & rbuf_new, unsigned int end_new
                   , struct writer_state & wstate) {

  unsigned int begin_old = rbuf_old.last_output;
  unsigned int begin_new = rbuf_new.last_output;

  if(end_old > begin_old && end_new > begin_new) {

    if(is_white_space(nodes_old.at(begin_old)) && is_white_space(nodes_new.at(begin_new))) {

      xmlChar * content_old = nodes_old.at(begin_old)->content;
      xmlChar * content_new = nodes_new.at(begin_new)->content;

      int size_old = strlen((const char *)content_old);
      int size_new = strlen((const char *)content_new);

      int offset_old = 0;
      int offset_new = 0;

      for(; offset_old < size_old && offset_new < size_new && content_old[offset_old] == content_new[offset_new]; ++offset_old, ++offset_new)
        ;

      xmlTextWriterWriteRawLen(wstate.writer, content_old, offset_old);

      if(offset_old < size_old) {

        // shrink
        nodes_old.at(begin_old)->content = content_old + offset_old;
        //node_sets_old->at(begin_old)->at(0)->content = (xmlChar *)strndup((const char *)(content_old + offset_old), size_old - offset_old);

      } else {

        nodes_old.at(begin_old)->content = (xmlChar *)"";
      }

      if(offset_new < size_new) {

        nodes_new.at(begin_new)->content = content_new + offset_new;

      } else {

        nodes_new.at(begin_new)->content = (xmlChar *)"";
      }


    }

  }

  if(end_old > begin_old) {

    // output diff tag begin
    if(rbuf_old.open_diff.back()->operation != DELETE)
      output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);

    for(unsigned int i = begin_old; i < end_old; ++i)
      output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    rbuf_old.last_output = end_old;

  }

  if(end_new > begin_new) {

    // output diff tag
    if(rbuf_new.open_diff.back()->operation != INSERT)
      output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

    for(unsigned int i = begin_new; i < end_new; ++i)
      output_node(rbuf_old, rbuf_new, nodes_new.at(i), INSERT, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

    rbuf_new.last_output = end_new;

  }

}

void output_char(char character, struct writer_state & wstate) {

  if(character == '&')
    xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST (unsigned char*) "&amp;", 5);

  else if (character == '<')
    xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST (unsigned char*) "&lt;", 4);

  else if (character == '>')

    xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST (unsigned char*) "&gt;", 4);

  else
    xmlTextWriterWriteRawLen(wstate.writer, BAD_CAST (unsigned char*) &character, 1);

}

bool is_block_type(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  for(int i = 0; block_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, block_types[i]) == 0)
      return true;

  return false;
}

bool is_nest_type(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes) {

  if((xmlReaderTypes)nodes.at(structure->at(0))->type != XML_READER_TYPE_ELEMENT)
    return false;

  for(int i = 0; nest_types[i]; ++i)
    if(strcmp((const char *)nodes.at(structure->at(0))->name, nest_types[i]) == 0)
      return true;

  return false;
}

bool has_interal_block(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes) {

  if(strcmp((const char *)nodes.at(structure->at(0))->name, "block") == 0)
    return false;

  for(unsigned int i = 1; i < structure->size(); ++i)
    if(strcmp((const char *)nodes.at(structure->at(i))->name, "block") == 0)
      return true;

  return false;
}

bool is_nestable(std::vector<int> * structure_one, std::vector<xmlNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xmlNodePtr> & nodes_two) {

    
  if(is_nest_type(structure_one, nodes_one) && is_block_type(structure_two, nodes_two)) {

    if(strcmp((const char *)nodes_one.at(structure_one->at(0))->name, block_types[0]) != 0) {

      return true;

    } else {

      if(has_interal_block(structure_two, nodes_two))
        return true;
    
    }
  }

  return false;
}

void output_nested(struct reader_state rbuf_old, std::vector<int> * structure_old
                   , struct reader_state rbuf_new ,std::vector<int> * structure_new
                   , int operation, struct writer_state wstate) {

  // may need to markup common that does not output common blocks
  markup_whitespace(rbuf_old, structure_old->at(0), rbuf_new, structure_new->at(0), wstate);

  if(operation == DELETE) {

    // output diff tag begin
    if(rbuf_old.open_diff.back()->operation != DELETE)
      output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);
    fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
      unsigned int start;
      unsigned int end;
      unsigned int start_pos;
      unsigned int end_pos;
    if(has_interal_block(structure_old, nodes_old)) {

      for(start = 0; start < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "block") != 0; ++start)
        ;

      for(end = start + 1; end < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(end))->name, "block") != 0; ++end)
        ;

      start_pos = structure_old->at(start);
      end_pos = structure_old->at(end) - 1;

      if(strcmp((const char *)nodes_old.at(structure_new->at(0))->name, "block") != 0)
        start_pos += 2;
      else
        end_pos += 2;

    } else if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "for") != 0){

      for(start = 0; start < structure_old->size() 
	    && (nodes_old.at(structure_old->at(start))->type != XML_READER_TYPE_END_ELEMENT
		|| strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "condition") != 0); ++start)
          ;

        ++start;

        start_pos = structure_old->at(start) + 1;
        end_pos = structure_old->back();

      if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "if") == 0) {

        ++start_pos;
        --end_pos;

      } else {

      for(start = 0; start < structure_old->size() 
	    && (nodes_old.at(structure_old->at(start))->type != XML_READER_TYPE_END_ELEMENT
                 || strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "incr") != 0)
            && (strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "incr") != 0
                || !(nodes_old.at(start)->extra & 0x1)); ++start)
            ;

        start += 3;

        start_pos = structure_old->at(start) + 1;
        end_pos = structure_old->back();

      }
    }

      for(unsigned int i = 0; i < start_pos; ++i)
        output_node(rbuf_old, rbuf_new, nodes_old[i], DELETE, wstate);

      rbuf_old.last_output = start_pos;

      // collect subset of nodes
      std::vector<std::vector<int> *> next_node_set_old
        = create_node_set(&nodes_old, start_pos
                          , end_pos);

      std::vector<std::vector<int> *> next_node_set_new
        = create_node_set(&nodes_new,  structure_new->at(0)
                          , structure_new->back() + 1);
      
      output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

      markup_whitespace(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

      for(unsigned int i = end_pos; i < structure_old->back() + 1; ++i)
        output_node(rbuf_old, rbuf_new, nodes_old[i], DELETE, wstate);

      rbuf_old.last_output = structure_old->back() + 1;

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    markup_whitespace(rbuf_old, rbuf_old.last_output, rbuf_new, rbuf_new.last_output, wstate);



  } else {

    // output diff tag
    if(rbuf_new.open_diff.back()->operation != INSERT)
      output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

    //for(unsigned int i = begin_new; i < end_new; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_new.at(i), INSERT, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

    rbuf_new.last_output = structure_new->back() + 1;

  }
}
