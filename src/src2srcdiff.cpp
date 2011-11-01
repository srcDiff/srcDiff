/*
  Create srcdiff format from two src files.

  Michael J. Decker
  mjd52@zips.uakron.edu
*/

#include <stdio.h>
#include <fstream>
#include <string>
#include <vector>
#include "../bin/Options.hpp"
#include "../bin/srcMLTranslator.hpp"
#include "../bin/Language.hpp"
#include "shortest_edit_script.h"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

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
const char* DIFF_OLD = "diff:old";
const char* DIFF_NEW = "diff:new";
const char* DIFF_COMMON = "diff:common";

const char* output_encoding = "UTF-8";

const xmlChar* EDIFF_ATTRIBUTE = BAD_CAST "type";

const char* EDIFF_BEGIN = "start";
const char* EDIFF_END = "end";

xmlNodePtr diff_common_start;
xmlNodePtr diff_common_end;
xmlNodePtr diff_old_start;
xmlNodePtr diff_old_end;
xmlNodePtr diff_new_start;
xmlNodePtr diff_new_end;

// constant template for temporary file names
char * srcdiff_template = (char *)"srcdifftemp.XXXXXX";

xmlNs diff = { NULL, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", NULL };

// diff accessor function
const void * line_index(int idx, const void *s) {
  std::vector<const char *> & lines = *(std::vector<const char *> *)s;
  return lines[idx];
}

// diff comparison function
int line_compare(const void * e1, const void * e2) {
  const char * str1 = (const char *)e1;
  const char * str2 = (const char *)e2;

  return strcmp(str1, str2);
}

// diff node accessor function
const void * node_index(int idx, const void *s) {
  std::vector<xmlNode *> & nodes = *(std::vector<xmlNode *> *)s;
  return nodes[idx];
}

// diff node comparison function
int node_compare(const void * e1, const void * e2) {
  xmlNode * node1 = (xmlNode *)e1;
  xmlNode * node2 = (xmlNode *)e2;

  if(node1->type == node2->type && strcmp((const char *)node1->name, (const char *)node2->name) == 0) {

    // end if text node contents differ
    if((xmlReaderTypes)node1->type == XML_READER_TYPE_TEXT)
      return strcmp((const char *)node1->content, (const char *)node2->content);
    else
      return 0;
  }

  return 1;
}

// converts source code to srcML
void translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

struct tag {

  int marked;
  xmlNodePtr node;
};

struct open_diff {

  int operation;
  int offset;
  std::vector<struct tag *> * open_tags;

};

// stores information during xml Text Reader processing
struct reader_buffer {

  int stream_source;
  int line_number;
  unsigned char * characters;
  std::vector<xmlNode *> * buffer;
  std::vector<struct open_diff *> * open_diff;
  std::vector<struct open_diff *> * output_diff;

  std::vector<struct open_diff *> * delay_close;

};

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// compares a line supposed to be the same and output the correrct elements
void compare_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old,struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer);

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int operation, int end_line);

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, struct edit * edit, xmlTextWriterPtr writer);

// output a change
void output_double(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlTextWriterPtr writer);

void output_handler(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

bool output_peek(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

void update_diff_stack(std::vector<struct open_diff *> * open_diffs, xmlNodePtr node, int operation);

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    //    fprintf(stderr, "Usage: src2srcdiff oldFile newFile srcdiffFile\n");
    fprintf(stderr, "Usage: src2srcdiff oldFile newFile dir\n");
    return 1;
  }

  const char * srcdiff_file;
  srcdiff_file = "-";

  /*
    std::string * dcommon = new std::string("diff:common");
    std::string * dold = new std::string("diff:old");
    std::string * dnew = new std::string("diff:new");

    diff_common_start = new xmlNode;
    diff_common_start->name = (xmlChar *)dcommon->c_str();
    diff_common_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
    diff_common_start->extra = 0;

    diff_common_end = new xmlNode;
    diff_common_end->name = (xmlChar *)dcommon->c_str();
    diff_common_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
    diff_common_end->extra = 0;

    diff_old_start = new xmlNode;
    diff_old_start->name = (xmlChar *)dold->c_str();
    diff_old_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
    diff_old_start->extra = 0;

    diff_old_end = new xmlNode;
    diff_old_end->name = (xmlChar *)dold->c_str();
    diff_old_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
    diff_old_end->extra = 0;

    diff_new_start = new xmlNode;
    diff_new_start->name = (xmlChar *)dnew->c_str();
    diff_new_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
    diff_new_start->extra = 0;

    diff_new_end = new xmlNode;
    diff_new_end->name = (xmlChar *)dnew->c_str();
    diff_new_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
    diff_new_end->extra = 0;
  */

  /*
    Compute the differences between the two source files

  */

  // files used for differences
  std::vector<char *> lines1;
  std::vector<char *> lines2;

  std::string * buffer = new std::string();

  // gather file one
  std::ifstream file1;
  file1.open(argv[1]);

  getline(file1, *buffer);
  while(!file1.eof()) {
    lines1.push_back((char *)buffer->c_str());
    buffer = new std::string();
    getline(file1, *buffer);
  }

  if(*buffer != "") {

    lines1.push_back((char *)buffer->c_str());
    buffer = new std::string();
  }
  file1.close();

  // gather file 2
  std::ifstream file2;
  file2.open(argv[2]);

  getline(file2, *buffer);
  while(!file2.eof()) {
    lines2.push_back((char *)buffer->c_str());
    buffer = new std::string();
    getline(file2, *buffer);
  }

  if(*buffer != "") {

    lines2.push_back((char *)buffer->c_str());
    buffer = new std::string();
  }
  file2.close();

  // calculate the differences
  struct edit * edit_script;

  int distance = shortest_edit_script(lines1.size(), (void *)&lines1, lines2.size(), (void *)&lines2, line_compare, line_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    return distance;
  }

  /*
    Translate both files to srcML separately.
  */

  // create temporary file for srcML file one
  char * srcml_file_one = mktemp(strdup(srcdiff_template));
  if(srcml_file_one == NULL) {

    fprintf(stderr, "Tempfile failed\n");
    return 1;
  }

  // translate file one
  translate_to_srcML(argv[1], srcml_file_one, argv[3]);

  // create temporary file for srcML file two
  char * srcml_file_two = mktemp(strdup(srcdiff_template));
  if(srcml_file_two == NULL) {

    fprintf(stderr, "Tempfile failed\n");
    return 1;
  }

  // translate file two
  translate_to_srcML(argv[2], srcml_file_two, argv[3]);

  /*
    Create xmlreaders and the xmlwriter
  */

  xmlTextReaderPtr reader_old = NULL;
  xmlTextReaderPtr reader_new = NULL;

  xmlTextWriterPtr writer = NULL;

  {
    // create the reader for the old file
    reader_old = xmlNewTextReaderFilename(srcml_file_one);
    if (reader_old == NULL) {
      fprintf(stderr, "Unable to open file '%s' as XML", srcml_file_one);

      goto cleanup;
    }

    // create the reader for the new file
    reader_new = xmlNewTextReaderFilename(srcml_file_two);
    if (reader_new == NULL) {
      fprintf(stderr, "Unable to open file '%s' as XML", srcml_file_two);

      goto cleanup;
    }

    // create the writer
    writer = xmlNewTextWriterFilename(srcdiff_file, 0);
    if (writer == NULL) {
      fprintf(stderr, "Unable to open file '%s' as XML", srcdiff_file);

      goto cleanup;
    }

    // issue the xml declaration
    xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);

    std::vector<struct open_diff *> output_diff;
    struct open_diff * new_diff = new struct open_diff;
    new_diff->operation = COMMON;
    new_diff->open_tags = new std::vector<struct tag *>;
    output_diff.push_back(new_diff);

    // run through diffs adding markup
    int last_diff = 0;
    struct reader_buffer rbuf_old = { NULL };
    rbuf_old.stream_source = DELETE;
    rbuf_old.open_diff = new std::vector<struct open_diff *>;

    new_diff = new struct open_diff;
    new_diff->operation = COMMON;
    new_diff->open_tags = new std::vector<struct tag *>;
    rbuf_old.open_diff->push_back(new_diff);

    rbuf_old.output_diff = &output_diff;
    rbuf_old.delay_close = new std::vector<struct open_diff *>;
    xmlTextReaderRead(reader_old);

    struct reader_buffer rbuf_new = { NULL };
    rbuf_new.stream_source = INSERT;
    rbuf_new.open_diff = new std::vector<struct open_diff *>;

    new_diff = new struct open_diff;
    new_diff->operation = COMMON;
    new_diff->open_tags = new std::vector<struct tag *>;
    rbuf_new.open_diff->push_back(new_diff);

    rbuf_new.output_diff = &output_diff;
    rbuf_new.delay_close = new std::vector<struct open_diff *>;
    xmlTextReaderRead(reader_new);

    // create srcdiff unit
    xmlNodePtr unit = create_srcdiff_unit(reader_old, reader_new);

    // output srcdiff unit
    output_handler(&rbuf_old, &rbuf_new, unit, COMMON, writer);

    xmlTextReaderRead(reader_old);
    xmlTextReaderRead(reader_new);

    struct edit * edits = edit_script;
    for (; edits; edits = edits->next) {

      /*
      // output diff tag start
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
      if(rbuf_old.open_diff->back()->operation != COMMON)
      output_handler(&rbuf_old, &rbuf_new, diff_common_start, COMMON, writer);

      rbuf_old.open_diff->back()->open_tags->front()->marked = false;
      */

      // add preceeding unchanged
      if(edits->operation == DELETE)
        for(int j = last_diff; j < edits->offset_sequence_one; ++rbuf_old.line_number, ++rbuf_new.line_number, ++j)
          compare_same_line(&rbuf_old, reader_old, &rbuf_new, reader_new, writer);
      else
        for(int j = last_diff; j < edits->offset_sequence_one + 1; ++rbuf_old.line_number, ++rbuf_new.line_number, ++j)
          compare_same_line(&rbuf_old, reader_old, &rbuf_new, reader_new, writer);

      /*
        if(rbuf_old.open_diff->back()->operation == COMMON && rbuf_old.open_diff->size() > 1)
        rbuf_old.open_diff->back()->open_tags->front()->marked = true;

        output_handler(&rbuf_old, &rbuf_new, diff_common_end, COMMON, writer);
      */

      // detect and change
      struct edit * edit_next = edits->next;
      if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
         && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

        collect_difference(&rbuf_old, reader_old, DELETE, edits->offset_sequence_one + edits->length);

        collect_difference(&rbuf_new, reader_new, INSERT, edits->next->offset_sequence_two + edits->next->length);

        output_double(&rbuf_old, &rbuf_new, writer);

        last_diff = edits->offset_sequence_one + edits->length;
        edits = edits->next;
        continue;
      }

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:

        collect_difference(&rbuf_new, reader_new, INSERT, edits->offset_sequence_two + edits->length);
        output_single(&rbuf_old, &rbuf_new, edits, writer);

        last_diff = edits->offset_sequence_one + 1;
        break;

      case DELETE:

        collect_difference(&rbuf_old, reader_old, DELETE, edits->offset_sequence_one + edits->length);
        output_single(&rbuf_old, &rbuf_new, edits, writer);

        last_diff = edits->offset_sequence_one + edits->length;
        break;
      }

    }

    /*
    // output diff tag start
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
    if(rbuf_old.open_diff->back()->operation != COMMON)
    output_handler(&rbuf_old, &rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old.open_diff->back()->open_tags->front()->marked = false;
    */

    for(unsigned int j = last_diff; j < lines1.size(); ++rbuf_old.line_number, ++rbuf_new.line_number, ++j)
      compare_same_line(&rbuf_old, reader_old, &rbuf_new, reader_new, writer);

    /*
      if(rbuf_old.open_diff->back()->operation == COMMON && rbuf_old.open_diff->size() > 1)
      rbuf_old.open_diff->back()->open_tags->front()->marked = true;

      output_handler(&rbuf_old, &rbuf_new, diff_common_end, COMMON, writer);
    */
  }

  // output srcdiff unit
  outputNode(*getRealCurrentNode(reader_old), writer);

  // cleanup everything
 cleanup:

  if(reader_old)
    xmlFreeTextReader(reader_old);

  if(reader_new)
    xmlFreeTextReader(reader_new);

  if(writer) {

    xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
  }
  int status = 0;
  if(remove(srcml_file_one) == -1) {

    fprintf(stderr, "Remove temp file one failed\n");
    status = 1;
  }

  if(remove(srcml_file_two) == -1) {

    fprintf(stderr, "Remove temp file two failed\n");
    status = 1;
  }

  free_shortest_edit_script(edit_script);

  return status;
}

// converts source code to srcML
void translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir) {

  // register default language extensions
  Language::register_standard_file_extensions();

  // get language from file extension
  int language = Language::getLanguageFromFilename(source_file);

  // select basic options
  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML  | OPTION_LITERAL | OPTION_OPERATOR | OPTION_MODIFIER;

  // create translator object
  srcMLTranslator translator(language, srcml_file, options);

  // set input file (must be done)
  translator.setInput(source_file);

  // translate file
  translator.translate(NULL, dir, NULL, NULL, language);

  // close the input file
  translator.close();
}

// compares a line supposed to be the same and output the correrct elements
void compare_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old,struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer) {

  int not_done = 1;
  while(not_done) {

    if(strcmp((const char *)getRealCurrentNode(reader_old)->name, (const char *)getRealCurrentNode(reader_new)->name) != 0) {

      //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf_old->line_number);
      //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf_new->line_number);

      collect_difference(rbuf_old, reader_old, DELETE, rbuf_old->line_number + 1);

      collect_difference(rbuf_new, reader_new, INSERT, rbuf_new->line_number + 1);

      output_double(rbuf_old, rbuf_new, writer);

      --rbuf_old->line_number;
      --rbuf_new->line_number;

      return;

    }

    // look if in text node
    if(xmlTextReaderNodeType(reader_old) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader_old) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty
      if(!rbuf_old->characters) {
        rbuf_old->characters = (unsigned char *)xmlTextReaderConstValue(reader_old);
        rbuf_new->characters = (unsigned char *)xmlTextReaderConstValue(reader_new);
      }

      if(strlen((const char *)rbuf_old->characters) != strlen((const char *)rbuf_new->characters)) {

        collect_difference(rbuf_old, reader_old, DELETE, rbuf_old->line_number + 1);

        collect_difference(rbuf_new, reader_new, INSERT, rbuf_new->line_number + 1);

        output_double(rbuf_old, rbuf_new, writer);

        --rbuf_old->line_number;
        --rbuf_new->line_number;

        return;

      }

      // cycle through characters
      for (; (*rbuf_old->characters) != 0; ++rbuf_old->characters, ++rbuf_new->characters) {

        // escape characters or print out character
        if (*rbuf_old->characters == '&')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&amp;"));
        else if (*rbuf_old->characters == '<')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;"));
        else if (*rbuf_old->characters == '>')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&gt;"));
        else
          xmlTextWriterWriteRawLen(writer, rbuf_old->characters, 1);

        // increase new line count and exit
        if((*rbuf_old->characters) == '\n') {

          ++rbuf_old->characters;
          ++rbuf_new->characters;

          if(!(*rbuf_old->characters)) {

            rbuf_old->characters = NULL;
            not_done = xmlTextReaderRead(reader_old);

          }

          if(!(*rbuf_new->characters)) {

            rbuf_new->characters = NULL;
            xmlTextReaderRead(reader_new);

          }

          return;
        }
      }

      // end text node if finished and get next node
      if(!(*rbuf_old->characters)) {

        rbuf_old->characters = NULL;
        rbuf_new->characters = NULL;

        not_done = xmlTextReaderRead(reader_old);
        xmlTextReaderRead(reader_new);
      }
    }
    else {

      xmlNodePtr node = getRealCurrentNode(reader_old);

      if(strcmp((const char *)node->name, "unit") == 0)
        return;

      output_handler(rbuf_old, rbuf_new, node, COMMON, writer);

      // output non-text node and get next node
      not_done = xmlTextReaderRead(reader_old);
      xmlTextReaderRead(reader_new);
    }
  }
}

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int operation, int end_line) {

  // save beginning of characters
  unsigned char * characters_start = rbuf->characters;

  // allocate new buffer
  rbuf->buffer = new std::vector<xmlNode *>;

  int not_done = 1;
  while(not_done)

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty and set start of characters
      if(!rbuf->characters) {

        characters_start = (unsigned char *)xmlTextReaderConstValue(reader);
        rbuf->characters = characters_start;
      }

      // cycle through characters
      for (; (*rbuf->characters) != 0; ++rbuf->characters) {

        // separte non whitespace
        if((*rbuf->characters) != ' ' && (*rbuf->characters) != '\t' && (*rbuf->characters) != '\r' && (*rbuf->characters) != '\n') {

          // output previous whitespace
          if(rbuf->characters != characters_start) {
            xmlNode * text = new xmlNode;
            text->type = (xmlElementType)XML_READER_TYPE_TEXT;
            text->name = (const xmlChar *)"text";

            const char * content = strndup((const char *)characters_start, rbuf->characters  - characters_start);
            text->content = (xmlChar *)content;
            rbuf->buffer->push_back(text);

            characters_start = rbuf->characters;

          }

          while((*rbuf->characters) != 0 && (*rbuf->characters) != ' ' && (*rbuf->characters) != '\t' && (*rbuf->characters) != '\r' && (*rbuf->characters) != '\n')
            ++rbuf->characters;

          // output other
          xmlNode * text = new xmlNode;
          text->type = (xmlElementType)XML_READER_TYPE_TEXT;
          text->name = (const xmlChar *)"text";

          const char * content = strndup((const char *)characters_start, rbuf->characters  - characters_start);
          text->content = (xmlChar *)content;
          rbuf->buffer->push_back(text);

          characters_start = rbuf->characters;

          if(!*rbuf->characters)
            break;

        }

        // increase new line count and check if end of diff
        if((*rbuf->characters) == '\n') {

          ++rbuf->line_number;

          xmlNode * text = new xmlNode;
          text->type = (xmlElementType)XML_READER_TYPE_TEXT;
          text->name = (const xmlChar *)"text";

          const char * content = strndup((const char *)characters_start, (rbuf->characters + 1) - characters_start);
          text->content = (xmlChar *)content;
          rbuf->buffer->push_back(text);

          characters_start = rbuf->characters + 1;

          // check if end of diff and create text node for text fragment
          if(rbuf->line_number == end_line) {

            ++rbuf->characters;

            if(!(*rbuf->characters)) {

              rbuf->characters = NULL;
              not_done = xmlTextReaderRead(reader);
            }

            return;
          }

        }
      }

      // end and save text node if finished and get next node
      if(!(*rbuf->characters)) {

        // create new node and buffer it
        if(rbuf->characters != characters_start) {

          xmlNode * text = new xmlNode;
          text->type = (xmlElementType)XML_READER_TYPE_TEXT;
          text->name = (const xmlChar *)"text";

          const char * content = strdup((const char *)characters_start);
          text->content = (xmlChar *)content;
          rbuf->buffer->push_back(text);

        }

        rbuf->characters = NULL;

        not_done = xmlTextReaderRead(reader);
      }
    }
    else {

      xmlNodePtr node = getRealCurrentNode(reader);

      if(strcmp((const char *)node->name, "unit") == 0)
        return;

      // save non-text node and get next node
      rbuf->buffer->push_back(node);

      not_done = xmlTextReaderRead(reader);
    }

  ++rbuf->line_number;

}

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, struct edit * edit, xmlTextWriterPtr writer) {

  //fprintf(stderr, "HERE_SINGLE\n");

  struct reader_buffer * rbuf = edit->operation == DELETE ? rbuf_old : rbuf_new;

  // output starting diff tag
  if(edit->operation == DELETE) {

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

    /*
      if(rbuf->open_diff->back()->operation != DELETE)
      output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

      rbuf->open_diff->back()->open_tags->front()->marked = false;
    */

  }else {

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));
    /*
      if(rbuf->open_diff->back()->operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

      rbuf->open_diff->back()->open_tags->front()->marked = false;
    */
  }

  /*  xmlNodePtr node;
      if(rbuf->open_diff->size() > 1 && rbuf->open_diff->back()->operation == edit->operation)
      node = (*rbuf->open_diff)[rbuf->open_diff->size() - 2]->open_tags->back();
      else
      node = rbuf->open_diff->back()->open_tags->back();

      // output diff outputting until identified open tag
      */

  xmlNodePtr bnode = NULL;
  unsigned int i;
  for(i = 0; i < rbuf->buffer->size(); ++i) {


    bnode = (*rbuf->buffer)[i];

    output_handler(rbuf_old, rbuf_new, bnode, edit->operation, writer);

  }

  // output ending diff tags
  // output starting diff tag
  if(edit->operation == DELETE) {

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));
    /*
      if(rbuf->open_diff->back()->operation == DELETE)
      rbuf->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);
    */

  }else {

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));
    /*
      if(rbuf->open_diff->back()->operation == INSERT)
      rbuf->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
    */

  }

  /*
  // output remaining nodes on line
  for(; i < rbuf->buffer->size(); ++i)
  output_handler(rbuf_old, rbuf_new, (*rbuf->buffer)[i], edit->operation, writer);
  */

  //fprintf(stderr, "HERE\n");
}

// output a change
void output_double(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlTextWriterPtr writer) {

  //fprintf(stderr, "HERE_DOUBLE\n");

  struct edit * edit_script;
  int distance = shortest_edit_script(rbuf_old->buffer->size(), (void *)rbuf_old->buffer, rbuf_new->buffer->size(), (void *)rbuf_new->buffer, node_compare, node_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff = 0;
  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    /*
    // output diff tag start
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
    if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;
    */

    // add preceeding unchanged
    if(edits->operation == DELETE)
      for(int j = last_diff; j < edits->offset_sequence_one; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[j], COMMON, writer);

    else
      for(int j = last_diff; j < edits->offset_sequence_one + 1; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[j], COMMON, writer);

    /*
      if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);
    */

    // detect and change
    struct edit * edit_next = edits->next;
    if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
       && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

      //      fprintf(stderr, "HERE\n");

      // look for pure whitespace change
      if((*rbuf_old->buffer)[edits->offset_sequence_one]->type == (*rbuf_new->buffer)[edit_next->offset_sequence_two]->type
         && (xmlReaderTypes)(*rbuf_old->buffer)[edits->offset_sequence_one]->type == XML_READER_TYPE_TEXT
         && edits->length == 1 && edit_next->length == 1) {

        int whitespace_length_old = strspn((const char *)(*rbuf_old->buffer)[edits->offset_sequence_one]->content, " \t\r\n");
        int whitespace_length_new = strspn((const char *)(*rbuf_new->buffer)[edit_next->offset_sequence_two]->content, " \t\r\n");

        xmlChar * content_old = (*rbuf_old->buffer)[edits->offset_sequence_one]->content;
        xmlChar * content_new = (*rbuf_new->buffer)[edit_next->offset_sequence_two]->content;

        int size_old = strlen((const char *)(*rbuf_old->buffer)[edits->offset_sequence_one]->content);
        int size_new = strlen((const char *)(*rbuf_new->buffer)[edit_next->offset_sequence_two]->content);

        if(whitespace_length_old == size_old && whitespace_length_new == size_new) {

          int end_old = size_old - 1;
          int end_new = size_new - 1;

          while(end_old >= 0 && end_new >= 0 && content_old[end_old] == content_new[end_new]) {

            --end_old;
            --end_new;
          }

          if(end_old >= 0) {

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));

            xmlTextWriterWriteRawLen(writer, content_old, end_old + 1);

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));

          }

          if(end_new >= 0) {

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

            xmlTextWriterWriteRawLen(writer, content_new, end_new + 1);

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("/<diff:new>"));

          }

          xmlTextWriterWriteRawLen(writer, content_old + end_old + 1, size_old - (end_old + 1));

          last_diff = edits->offset_sequence_one + edits->length;
          edits = edits->next;
          continue;
        }

      }

      /*
        if(rbuf_old->output_diff->back()->operation == DELETE || rbuf_old->output_diff->back()->operation == COMMON) {
      */

      // output diff tag start
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
      /*
        if(rbuf_old->open_diff->back()->operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

        rbuf_old->open_diff->back()->open_tags->front()->marked = false;
      */

      for(int j = 0; j < edits->length; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[edits->offset_sequence_one + j], DELETE, writer);

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));
      /*
      // output diff tag start
      if(rbuf_old->open_diff->back()->operation == DELETE)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;
      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);
      */

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));
      /*
      // output diff tag
      if(rbuf_new->open_diff->back()->operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

      rbuf_new->open_diff->back()->open_tags->front()->marked = false;
      */

      for(int j = 0; j < edit_next->length; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_new->buffer)[edit_next->offset_sequence_two + j], INSERT, writer);

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

      /*
      // output diff tag start
      if(rbuf_new->open_diff->back()->operation == INSERT)
      rbuf_new->open_diff->back()->open_tags->front()->marked = true;
      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
      */
      /*
        } else {

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));

        for(int j = 0; j < edit_next->length; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_new->buffer)[edit_next->offset_sequence_two + j], INSERT, writer);


        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

        for(int j = 0; j < edits->length; ++j)
        output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[edits->offset_sequence_one + j], DELETE, writer);

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

        }
      */


      last_diff = edits->offset_sequence_one + edits->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:

      {

        //      fprintf(stderr, "HERE\n");
        /*
          bool output_start = false;
          bool output_end = true;

          for(int j = 0; j < edits->length; ++j) {

          if(!output_start && output_peek(rbuf_old, rbuf_new, (*rbuf_new->buffer)[edits->offset_sequence_two + j], INSERT, writer)) {

          output_start = true;

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));
          }

          if(output_start && output_end && rbuf_new->open_diff->back()->operation == COMMON
          && (*rbuf_new->buffer)[edits->offset_sequence_two + j]->type == XML_READER_TYPE_END_ELEMENT) {

          output_end = false;

          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));
          }

          output_handler(rbuf_old, rbuf_new, (*rbuf_new->buffer)[edits->offset_sequence_two + j], INSERT, writer);

          }

          if(output_start && output_end)

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));
        */

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));

        /*
          if(rbuf_new->open_diff->back()->operation != INSERT)
          output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

          rbuf_new->open_diff->back()->open_tags->front()->marked = false;
        */

        for(int j = 0; j < edits->length; ++j)
          output_handler(rbuf_old, rbuf_new, (*rbuf_new->buffer)[edits->offset_sequence_two + j], INSERT, writer);

        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

        /*
        // output diff tag
        if(rbuf_new->open_diff->back()->operation == INSERT)
        rbuf_new->open_diff->back()->open_tags->front()->marked = true;
        output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
        */

        last_diff = edits->offset_sequence_one + 1;

      }

      break;

    case DELETE:

      {

        //fprintf(stderr, "HERE\n");

        /*
          bool output_start = false;
          bool output_end = true;

          for(int j = 0; j < edits->length; ++j) {

          if(!output_start && output_peek(rbuf_old, rbuf_new, (*rbuf_old->buffer)[edits->offset_sequence_one + j], DELETE, writer)) {

          output_start = true;

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
          }

          if(output_start && output_end && rbuf_old->open_diff->back()->operation == COMMON
          && (*rbuf_old->buffer)[edits->offset_sequence_one + j]->type == XML_READER_TYPE_END_ELEMENT) {

          output_end = false;
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

          }

          output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[edits->offset_sequence_one + j], DELETE, writer);

          }

          if(output_start && output_end)

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));
        */

        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

        /*
        // output diff tag start
        if(rbuf_old->open_diff->back()->operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

        rbuf_old->open_diff->back()->open_tags->front()->marked = false;
        */

        for(int j = 0; j < edits->length; ++j)
          output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[edits->offset_sequence_one + j], DELETE, writer);

        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

        /*
        // output diff tag
        if(rbuf_old->open_diff->back()->operation == DELETE)
        rbuf_old->open_diff->back()->open_tags->front()->marked = true;
        output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);
        */

        last_diff = edits->offset_sequence_one + edits->length;

      }

      break;
    }

  }

  /*
  // output diff tag start
  //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
  if(rbuf_old->open_diff->back()->operation != COMMON)
  output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;
  */

  for(unsigned int j = last_diff; j < rbuf_old->buffer->size(); ++j)
    output_handler(rbuf_old, rbuf_new, (*rbuf_old->buffer)[j], COMMON, writer);


  /*
    if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);
  */

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
    for(; namespaces->next; namespaces = namespaces->next);

    namespaces->next = ns;
  }
  else
    *nsDef = ns;

}

void update_diff_stack(std::vector<struct open_diff *> * open_diffs, xmlNodePtr node, int operation) {

  if(node->extra & 0x1)
    return;

  if(open_diffs->back()->operation != operation) {

    struct open_diff * new_diff = new struct open_diff;
    new_diff->operation = operation;
    new_diff->open_tags = new std::vector<struct tag *>;

    open_diffs->push_back(new_diff);
  }

  //xmlNodePtr node = getRealCurrentNode(reader);
  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    struct tag * new_tag = new struct tag;
    new_tag->marked = false;
    new_tag->node = node;

    open_diffs->back()->open_tags->push_back(new_tag);
  } else if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if(open_diffs->size() == 1 && open_diffs->back()->open_tags->size() == 1)
      return;


    open_diffs->back()->open_tags->pop_back();
  }


  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs->size());
  //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs->back()->open_tags->size());
  if(open_diffs->back()->open_tags->size() == 0) {
    open_diffs->pop_back();

    //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs->size());
    //fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, open_diffs->back()->open_tags->size());
  }
  //fprintf(stderr, "HERE\n");

}

void output_handler(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer) {

  /*
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, operation);
    fprintf(stderr, "HERE: %s %s %d %d\n", __FILE__, __FUNCTION__, __LINE__, rbuf->output_diff->back()->operation);

    if(node->type == XML_READER_TYPE_TEXT)
    fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->content);
    else
    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);
  */

  struct reader_buffer * rbuf = operation == DELETE ? rbuf_old : rbuf_new;

  if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {
    //    fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    // heavily relaxed condition.  Tighten when fix diff to contain necessary elements
    // mark nodes for output
    int unmarked;
    for(unmarked = rbuf_old->open_diff->back()->open_tags->size() - 1; unmarked > 0
          && (*rbuf_old->open_diff->back()->open_tags)[unmarked]->marked; --unmarked);

    if(rbuf->output_diff->back()->operation != COMMON) {

      if(strcmp((const char *)(*rbuf_old->open_diff->back()->open_tags)[unmarked]->node->name, (const char *)node->name) == 0)
        (*rbuf_old->open_diff->back()->open_tags)[unmarked]->marked = true;

    } else {

      if(operation != INSERT)
        if(strcmp((const char *)(*rbuf_old->open_diff->back()->open_tags)[unmarked]->node->name, (const char *)node->name) == 0)
          (*rbuf_old->open_diff->back()->open_tags)[unmarked]->marked = true;

    }

    for(unmarked = rbuf_new->open_diff->back()->open_tags->size() - 1; unmarked > 0
          && (*rbuf_new->open_diff->back()->open_tags)[unmarked]->marked; --unmarked);

    if(rbuf->output_diff->back()->operation != COMMON) {

      if(strcmp((const char *)(*rbuf_new->open_diff->back()->open_tags)[unmarked]->node->name, (const char *)node->name) == 0)
        (*rbuf_new->open_diff->back()->open_tags)[unmarked]->marked = true;

    } else {

      if(operation != DELETE)
        if(strcmp((const char *)(*rbuf_new->open_diff->back()->open_tags)[unmarked]->node->name, (const char *)node->name) == 0)
          (*rbuf_new->open_diff->back()->open_tags)[unmarked]->marked = true;

    }

    // output marked nodes
    while(1) {

      if(rbuf->output_diff->back()->operation == COMMON
         && (rbuf_old->open_diff->back()->operation == COMMON
             && rbuf_new->open_diff->back()->operation == COMMON)
         && (!rbuf_old->open_diff->back()->open_tags->back()->marked
             && !rbuf_new->open_diff->back()->open_tags->back()->marked))
        return;

      if(rbuf->output_diff->back()->operation == DELETE
         && rbuf_old->open_diff->back()->operation == DELETE
         && !rbuf_old->open_diff->back()->open_tags->back()->marked)
        return;

      if(rbuf->output_diff->back()->operation == INSERT
         && rbuf_new->open_diff->back()->operation == INSERT
         && !rbuf_new->open_diff->back()->open_tags->back()->marked)
        return;

      // output non-text node and get next node
      xmlNodePtr output_node;
      if(strcmp((const char *)rbuf->output_diff->back()->open_tags->back()->node->name, "diff:old") == 0)
        output_node = diff_old_end;
      else if(strcmp((const char *)rbuf->output_diff->back()->open_tags->back()->node->name, "diff:new") == 0)
        output_node = diff_new_end;
      else {

        output_node = rbuf->output_diff->back()->open_tags->back()->node;
        output_node->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
      }

      outputNode(*output_node, writer);

      if(rbuf->output_diff->back()->operation == COMMON) {

        //fprintf(stderr, "HERE OUTPUT COMMON\n");

        update_diff_stack(rbuf_old->open_diff, node, COMMON);
        update_diff_stack(rbuf_new->open_diff, node, COMMON);

        update_diff_stack(rbuf_old->output_diff, node, COMMON);

      }
      else if(rbuf->output_diff->back()->operation == DELETE) {

        //fprintf(stderr, "HERE OUTPUT DELETE\n");
        //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

        update_diff_stack(rbuf_old->open_diff, node, DELETE);

        update_diff_stack(rbuf_old->output_diff, node, DELETE);

      } else {

        //fprintf(stderr, "HERE OUTPUT INSERT\n");
        //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

        update_diff_stack(rbuf_new->open_diff, node, INSERT);

        update_diff_stack(rbuf_new->output_diff, node, INSERT);
      }

    }

    return;
  }

  // output non-text node and get next node
  outputNode(*node, writer);

  if(operation == COMMON) {

    //fprintf(stderr, "HERE OUTPUT COMMON\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old->open_diff, node, operation);
    update_diff_stack(rbuf_new->open_diff, node, operation);

    update_diff_stack(rbuf_old->output_diff, node, operation);

  }
  else if(operation == DELETE) {

    //fprintf(stderr, "HERE OUTPUT DELETE\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_old->open_diff, node, operation);

    update_diff_stack(rbuf_old->output_diff, node, operation);

  } else {

    //fprintf(stderr, "HERE OUTPUT INSERT\n");
    //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, (const char *)node->name);

    update_diff_stack(rbuf_new->open_diff, node, operation);

    update_diff_stack(rbuf_new->output_diff, node, operation);
  }

}

bool output_peek(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer) {

  //fprintf(stderr, "HERE PEAK\n");

  return true;

}
