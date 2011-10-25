/*
  src2srcml.cpp

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
#define COMMON -1
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

// constant template for temporary file names
char * srcdiff_template = (char *)"srcdifftemp.XXXXXX";

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

xmlNs diff =  { NULL, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", NULL};

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

// stores information during xml Text Reader processing
struct reader_buffer {

  int line_number;
  unsigned char * characters;
  std::vector<xmlNode *> * buffer;
  std::vector<int> * in_diff;
  std::vector<bool> * issued_diff;
  std::vector<xmlNode *> * context;
};

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// compares a line supposed to be the same and output the correrct elements
void compare_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old, struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer, std::vector<int> * open_diff, int end_line);

void merge_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old, struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer, std::vector<int> * open_diff, int end_line);

void output_single(struct reader_buffer * rbuf, xmlTextReaderPtr reader, struct reader_buffer * rbuf_other, xmlTextWriterPtr writer, std::vector<int> * open_diff, int operation, int end_line);

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int operation, int end_line);

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf, struct edit * edit, xmlTextWriterPtr writer);

// output a change
void output_double(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlTextWriterPtr writer);

void update_context(struct reader_buffer * rbuf, xmlTextReaderPtr reader);

void update_in_diff(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int indiff);

void update_issued_diff(struct reader_buffer * rbuf, xmlTextReaderPtr reader);

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

    std::vector<int> open_diff;

    // run through diffs adding markup
    int last_diff = 0;
    struct reader_buffer rbuf_old = { NULL };
    rbuf_old.context = new std::vector<xmlNode *>;
    rbuf_old.in_diff = new std::vector<int>;
    rbuf_old.issued_diff = new std::vector<bool>;
    xmlTextReaderRead(reader_old);

    struct reader_buffer rbuf_new = { NULL };
    rbuf_new.context = new std::vector<xmlNode *>;
    rbuf_new.in_diff = new std::vector<int>;
    rbuf_new.issued_diff = new std::vector<bool>;
    xmlTextReaderRead(reader_new);

    // create srcdiff unit
    xmlNodePtr unit = create_srcdiff_unit(reader_old, reader_new);

    // output srcdiff unit
    outputNode(*unit, writer);

    update_context(&rbuf_old, reader_old);
    update_in_diff(&rbuf_old, reader_old, COMMON);
    update_issued_diff(&rbuf_old, reader_old);
    update_context(&rbuf_new, reader_new);
    update_in_diff(&rbuf_new, reader_new, COMMON);
    update_issued_diff(&rbuf_new, reader_new);

    open_diff.push_back(COMMON);

    xmlTextReaderRead(reader_old);
    xmlTextReaderRead(reader_new);

    struct edit * edits = edit_script;
    for (; edits; edits = edits->next) {

      // add preceeding unchanged
      if(edits->operation == DELETE)
        compare_same_line(&rbuf_old, reader_old, &rbuf_new, reader_new, writer, &open_diff, edits->offset_sequence_one);
      else
        compare_same_line(&rbuf_old, reader_old, &rbuf_new, reader_new, writer, &open_diff, edits->offset_sequence_one + 1);


      // detect and change
      struct edit * edit_next = edits->next;
      if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
         && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

        if(open_diff.back() == INSERT) {

          output_single(&rbuf_new, reader_new, &rbuf_old, writer, &open_diff, INSERT, edit_next->offset_sequence_two + edits->length);
          output_single(&rbuf_old, reader_old, &rbuf_new, writer, &open_diff, DELETE, edits->offset_sequence_one + edits->length);
        } else {

          output_single(&rbuf_old, reader_old, &rbuf_new, writer, &open_diff, DELETE, edits->offset_sequence_one + edits->length);
          output_single(&rbuf_new, reader_new, &rbuf_old, writer, &open_diff, INSERT, edit_next->offset_sequence_two + edits->length);
        }

        last_diff = edits->offset_sequence_one + edits->length;
        edits = edits->next;
        continue;
      }

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:

        output_single(&rbuf_new, reader_new, &rbuf_old, writer, &open_diff, INSERT, edits->offset_sequence_two + edits->length);

        last_diff = edits->offset_sequence_one + 1;
        break;

      case DELETE:

        output_single(&rbuf_old, reader_old, &rbuf_new, writer, &open_diff, DELETE, edits->offset_sequence_one + edits->length);

        last_diff = edits->offset_sequence_one + edits->length;
        break;
      }

    }

    compare_same_line(&rbuf_old, reader_old, &rbuf_new, reader_new, writer, &open_diff, lines1.size());

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

// compares a line supposed to be the same and output the correct elements
void compare_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old, struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer, std::vector<int> * open_diff, int end_line) {

  if(end_line == 0)
    return;

  unsigned int last_open_old;
  for(last_open_old = (rbuf_old->in_diff->size() - 1); last_open_old > 0 && (*rbuf_old->in_diff)[last_open_old] == COMMON; --last_open_old);

  ++last_open_old;

  unsigned int last_open_new;
  for(last_open_new = (rbuf_new->in_diff->size() - 1); last_open_new > 0 && (*rbuf_new->in_diff)[last_open_new] == COMMON; --last_open_new);

  ++last_open_new;

  bool mark_open = false;
  if((last_open_old > 1 && last_open_old == rbuf_old->in_diff->size())
     || (last_open_new > 1 && last_open_new == rbuf_new->in_diff->size())) {

    mark_open = true;
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:common>"));
    open_diff->push_back(COMMON);
  }

  int not_done = 1;
  int output_type = COMMON;
  int output_end = -2;
  while(not_done) {

    if(strcmp((const char *)getRealCurrentNode(reader_old)->name, (const char *)getRealCurrentNode(reader_new)->name) != 0) {
 
      merge_same_line(rbuf_old, reader_old, rbuf_new, reader_new, writer, open_diff, rbuf_old->line_number + 1);

    }

    // look if in text node
    if(xmlTextReaderNodeType(reader_old) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader_old) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty
      if(!rbuf_old->characters) {
        rbuf_old->characters = (unsigned char *)xmlTextReaderConstValue(reader_old);
        rbuf_new->characters = (unsigned char *)xmlTextReaderConstValue(reader_new);
      }

      // cycle through characters
      for (; *rbuf_old->characters != 0; ++rbuf_old->characters, ++rbuf_new->characters) {

        // escape characters or print out character
        if (*rbuf_old->characters == '&')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&amp;"));
        else if (*rbuf_old->characters == '<')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;"));
        else if (*rbuf_old->characters == '>')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&gt;"));
        else
          xmlTextWriterWriteRawLen(writer, rbuf_old->characters, 1);

        // increase new line count and check if done
        if((*rbuf_old->characters) == '\n') {

          ++rbuf_old->line_number;
          ++rbuf_new->line_number;

          if(rbuf_old->line_number == end_line) {

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

            if(mark_open)
              xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:common>"));

            open_diff->pop_back();

            return;
          }

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

      if((output_type == COMMON || output_type == DELETE) && output_end == (signed)rbuf_old->issued_diff->size() - 1
         || (output_type == COMMON || output_type == INSERT) && output_end == (signed)rbuf_new->issued_diff->size() - 1) {

        mark_open = false;
        output_end = -2;

        if(output_type == DELETE)
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
        else if(output_type == INSERT)
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));
        else
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:common>"));

        open_diff->pop_back();

      }

      if(strcmp((const char *)getRealCurrentNode(reader_old)->name, "unit") == 0)
        return;

      outputNode(*getRealCurrentNode(reader_old), writer);

      if(rbuf_old->issued_diff->back() && (xmlReaderTypes)getRealCurrentNode(reader_old)->type == XML_READER_TYPE_END_ELEMENT) {

        output_type = rbuf_old->in_diff->back();

        if(output_type == COMMON) {

          mark_open = true;
          output_end = rbuf_old->issued_diff->size() - 2;
        }
        else {

          // first two lines were not here not sure why not
          mark_open = false;
          output_end = -2;
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
          open_diff->pop_back();
        }
      }

      if(rbuf_new->issued_diff->back() && (xmlReaderTypes)getRealCurrentNode(reader_new)->type == XML_READER_TYPE_END_ELEMENT) {

        output_type = rbuf_new->in_diff->back();

        if(output_type == COMMON) {

          mark_open = true;
          output_end = rbuf_new->issued_diff->size() - 2;
        }
        else {

          mark_open = false;
          output_end = -2;
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));
          open_diff->pop_back();
        }

      }

      update_context(rbuf_old, reader_old);
      update_in_diff(rbuf_old, reader_old, COMMON);
      update_issued_diff(rbuf_old, reader_old);
      update_context(rbuf_new, reader_new);
      update_in_diff(rbuf_new, reader_new, COMMON);
      update_issued_diff(rbuf_new, reader_new);

      if(mark_open && (xmlReaderTypes)getRealCurrentNode(reader_old)->type == XML_READER_TYPE_ELEMENT) {

        mark_open = false;
        (*rbuf_old->issued_diff)[rbuf_old->issued_diff->size() - 1] = true;
        (*rbuf_new->issued_diff)[rbuf_new->issued_diff->size() - 1] = true;
        output_end = -2;

      }

      not_done = xmlTextReaderRead(reader_old);
      xmlTextReaderRead(reader_new);

    }
  }

  ++rbuf_old->line_number;
  ++rbuf_new->line_number;

}

void merge_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old, struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer, std::vector<int> * open_diff, int end_line) {

  int not_done_old = 1;
  if((xmlReaderTypes)getRealCurrentNode(reader_old)->type == XML_READER_TYPE_ELEMENT) {

    // Output old then node then common
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));
    open_diff->push_back(DELETE);

    outputNode(*getRealCurrentNode(reader_old), writer);

    update_context(rbuf_old, reader_old);
    update_in_diff(rbuf_old, reader_old, DELETE);
    update_issued_diff(rbuf_old, reader_old);

    not_done_old = xmlTextReaderRead(reader_old);
    
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:common>"));
    open_diff->push_back(COMMON);

  }

  int not_done_new = 1;
  if((xmlReaderTypes)getRealCurrentNode(reader_new)->type == XML_READER_TYPE_ELEMENT) {

    // Output new then node then common
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));
    open_diff->push_back(INSERT);

    outputNode(*getRealCurrentNode(reader_new), writer);

    update_context(rbuf_new, reader_new);
    update_in_diff(rbuf_new, reader_new, INSERT);
    update_issued_diff(rbuf_new, reader_new);

    not_done_new = xmlTextReaderRead(reader_new);
    
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:common>"));
    open_diff->push_back(COMMON);

  }

  // check if that open corrected and they now match if do return
  if(strcmp((const char *)getRealCurrentNode(reader_old)->name, (const char *)getRealCurrentNode(reader_new)->name) == 0)
    return;

  // check last open diff and use to decide which goes first
  if(open_diff->back() == INSERT) {

    // while in close and closing old or new element continue
    while(not_done_new && !rbuf_new->issued_diff->back() && rbuf_new->in_diff->back() == INSERT 
          && (xmlReaderTypes)getRealCurrentNode(reader_new)->type == XML_READER_TYPE_END_ELEMENT) {

      // update and output stuff
      outputNode(*getRealCurrentNode(reader_new), writer);

      update_context(rbuf_new, reader_new);
      update_in_diff(rbuf_new, reader_new, INSERT);
      update_issued_diff(rbuf_new, reader_new);

      not_done_new = xmlTextReaderRead(reader_new);

    }

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));
      open_diff->pop_back();
      

    // while in close and closing old or new element continue
    while(not_done_old && !rbuf_old->issued_diff->back() && rbuf_old->in_diff->back() == DELETE) {

      // update and output stuff
      outputNode(*getRealCurrentNode(reader_old), writer);

      update_context(rbuf_old, reader_old);
      update_in_diff(rbuf_old, reader_old, INSERT);
      update_issued_diff(rbuf_old, reader_old);

      not_done_old = xmlTextReaderRead(reader_old);

    }

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
      open_diff->pop_back();
      

  } else {

    // while in close and closing old or new element continue
    while(not_done_old && !rbuf_old->issued_diff->back() && rbuf_old->in_diff->back() == DELETE) {

      // update and output stuff
      outputNode(*getRealCurrentNode(reader_old), writer);

      update_context(rbuf_old, reader_old);
      update_in_diff(rbuf_old, reader_old, INSERT);
      update_issued_diff(rbuf_old, reader_old);

      not_done_old = xmlTextReaderRead(reader_old);

    }

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
      open_diff->pop_back();
      

    // while in close and closing old or new element continue
    while(not_done_new && !rbuf_new->issued_diff->back() && rbuf_new->in_diff->back() == INSERT) {

      // update and output stuff
      outputNode(*getRealCurrentNode(reader_new), writer);

      update_context(rbuf_new, reader_new);
      update_in_diff(rbuf_new, reader_new, INSERT);
      update_issued_diff(rbuf_new, reader_new);

      not_done_new = xmlTextReaderRead(reader_new);

    }

      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));
      open_diff->pop_back();
      

  }

}


void output_single(struct reader_buffer * rbuf, xmlTextReaderPtr reader, struct reader_buffer * rbuf_other, xmlTextWriterPtr writer, std::vector<int> * open_diff, int operation, int end_line) {

  if(end_line == 0)
    return;

  unsigned int last_open;
  for(last_open = (rbuf->in_diff->size() - 1); last_open > 0 && (*rbuf->in_diff)[last_open] == operation; --last_open);

  ++last_open;

  bool mark_open = false;
  if(last_open == rbuf->in_diff->size()) {

    mark_open = true;

    if(operation == DELETE)
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));
    else
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

    open_diff->push_back(operation);
  }

  unsigned int last_open_other;
  for(last_open_other = (rbuf_other->in_diff->size() - 1); last_open_other > 0 && (*rbuf_other->in_diff)[last_open_other] == COMMON; --last_open_other);

  ++last_open_other;

  int not_done = 1;
  int output_type = operation;
  int output_end = -2;
  while(not_done) {

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty
      if(!rbuf->characters) {
        rbuf->characters = (unsigned char *)xmlTextReaderConstValue(reader);
      }

      // cycle through characters
      for (; *rbuf->characters != 0; ++rbuf->characters) {

        // escape characters or print out character
        if (*rbuf->characters == '&')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&amp;"));
        else if (*rbuf->characters == '<')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&lt;"));
        else if (*rbuf->characters == '>')
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("&gt;"));
        else
          xmlTextWriterWriteRawLen(writer, rbuf->characters, 1);

        // increase new line count and check if done
        if((*rbuf->characters) == '\n') {

          ++rbuf->line_number;

          if(rbuf->line_number == end_line) {

            ++rbuf->characters;

            if(!(*rbuf->characters)) {

              rbuf->characters = NULL;
              not_done = xmlTextReaderRead(reader);

            }

            if(mark_open)
              if(output_type == DELETE)
                xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
              else
                xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

            open_diff->pop_back();
    
            return;
          }

        }

      }

      // end text node if finished and get next node
      if(!(*rbuf->characters)) {

        rbuf->characters = NULL;

        not_done = xmlTextReaderRead(reader);
      }
    }
    else {

      if(output_end == (signed)rbuf->issued_diff->size() - 1) {

        mark_open = false;
        output_end = -2;

        if(output_type == DELETE)
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
        else if(output_type == INSERT)
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));
        else
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:common>"));

            open_diff->pop_back();

      }

      if(strcmp((const char *)getRealCurrentNode(reader)->name, "unit") == 0)
        return;

      outputNode(*getRealCurrentNode(reader), writer);

      if(rbuf->issued_diff->back() && (xmlReaderTypes)getRealCurrentNode(reader)->type == XML_READER_TYPE_END_ELEMENT) {

        output_type = rbuf->in_diff->back();

        if(output_type != COMMON) {

          mark_open = true;
          output_end = rbuf->issued_diff->size() - 2;
        }
        else {

          // must end both streams if common
          if(rbuf_other->issued_diff->back()
              && strcmp((const char *)rbuf->context->back()->name, (const char *)rbuf_other->context->back()->name) == 0) {

            mark_open = false;
            output_end = -2;
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:common>"));

            open_diff->pop_back();

            if(rbuf_other->context->size() != 1) {

              rbuf_other->context->pop_back();
              rbuf_other->in_diff->pop_back();
              rbuf_other->issued_diff->pop_back();
            }
          }
        }

        }else if(rbuf->in_diff->back() == COMMON 
                 && (xmlReaderTypes)getRealCurrentNode(reader)->type == XML_READER_TYPE_END_ELEMENT
                 && strcmp((const char *)rbuf->context->back()->name, (const char *)rbuf_other->context->back()->name) == 0) {

          if(rbuf_other->context->size() != 1) {

            rbuf_other->context->pop_back();
            rbuf_other->in_diff->pop_back();
            rbuf_other->issued_diff->pop_back();
          }
        }

      update_context(rbuf, reader);
      update_in_diff(rbuf, reader, operation);
      update_issued_diff(rbuf, reader);

      if(mark_open && (xmlReaderTypes)getRealCurrentNode(reader)->type == XML_READER_TYPE_ELEMENT) {

        mark_open = false;
        (*rbuf->issued_diff)[rbuf->issued_diff->size() - 1] = true;
        output_end = -2;

      }

      // output non-text node and get next node
      not_done = xmlTextReaderRead(reader);

    }
  }

  ++rbuf->line_number;

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
      for (; *rbuf->characters != 0; ++rbuf->characters) {

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

          while(*rbuf->characters != 0 && (*rbuf->characters) != ' ' && (*rbuf->characters) != '\t' && (*rbuf->characters) != '\r' && (*rbuf->characters) != '\n')
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

      if(strcmp((const char *)getRealCurrentNode(reader)->name, "unit") == 0)
        break;

      // do not update closes.
      if((xmlReaderTypes)getRealCurrentNode(reader)->type == XML_READER_TYPE_ELEMENT) {

        update_context(rbuf, reader);
        update_in_diff(rbuf, reader, operation);
        update_issued_diff(rbuf, reader);

      }

      // save non-text node and get next node
      rbuf->buffer->push_back(getRealCurrentNode(reader));

      not_done = xmlTextReaderRead(reader);
    }

  ++rbuf->line_number;

}

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf, struct edit * edit, xmlTextWriterPtr writer) {

  // may be complete whitespace need to check for that
  // find if in same diff type
  // find the last open tag that needs to be closed
  unsigned int last_open;
  for(last_open = (rbuf->in_diff->size() - 1); last_open > 0 && (*rbuf->in_diff)[last_open] == edit->operation; --last_open);

  bool issued_diff = (last_open + 1) != rbuf->in_diff->size() && !(*rbuf->issued_diff)[last_open + 1];

  if((last_open + 1) != rbuf->in_diff->size())
    ++last_open;

  if(issued_diff)
    (*rbuf->issued_diff)[last_open] = true;

  // output starting diff tag
  if(issued_diff && edit->operation == DELETE)

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));
  else if(issued_diff)

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

  //last_open;
  xmlNodePtr node = (*rbuf->context)[last_open];

  // count number same open
  int count = 1;
  for(unsigned int i = last_open + 1; i < rbuf->context->size(); ++i) {

    xmlNodePtr bnode = (*rbuf->context)[i];

    if((xmlReaderTypes)bnode->type == XML_READER_TYPE_END_ELEMENT && strcmp((const char *)node->name, (const char *)bnode->name) == 0)
      ++count;

  }

  // output diff outputting until identified open tag
  xmlNodePtr bnode = NULL;
  unsigned int i;

  bool output_diff = false;

  for(i = 0; i < rbuf->buffer->size(); ++i) {

    bnode = (*rbuf->buffer)[i];

    outputNode(*bnode, writer);

    if((xmlReaderTypes)bnode->type == XML_READER_TYPE_END_ELEMENT && strcmp((const char *)node->name, (const char *)bnode->name) == 0) {

      --count;
      if(count == 0) {

        ++i;
        output_diff = true;
        break;
      }

    }

  }

  // may need to be output until close
  // check if last node is text node and output
  if(i == (rbuf->buffer->size() - 1)) {
    bnode = (*rbuf->buffer)[i];
    if(bnode && (xmlReaderTypes)bnode->type == XML_READER_TYPE_TEXT) {

      ++i;
      outputNode(*bnode, writer);
    }
  }

  // output ending diff tags
  if(output_diff)
    if(edit->operation == DELETE)

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
    else

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

  // check if in common may need to issue new common
  // output remaining nodes on line
  for(; i < rbuf->buffer->size(); ++i)
    outputNode(*(*rbuf->buffer)[i], writer);


  for(i = 0; i < rbuf->buffer->size(); ++i)
    if((xmlReaderTypes)(*rbuf->buffer)[i]->type == XML_READER_TYPE_END_ELEMENT) {

      rbuf->context->pop_back();
      rbuf->in_diff->pop_back();
      rbuf->issued_diff->pop_back();

    }
}

// output a change
void output_double(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlTextWriterPtr writer) {

  struct edit * edit_script;
  int distance = shortest_edit_script(rbuf_old->buffer->size(), (void *)rbuf_old->buffer, rbuf_new->buffer->size(), (void *)rbuf_new->buffer, node_compare, node_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff = 0;
  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    // add preceeding unchanged
    if(edits->operation == DELETE)
      for(int j = last_diff; j < edits->offset_sequence_one; ++j)
        outputNode(*(*rbuf_old->buffer)[j], writer);

    else
      for(int j = last_diff; j < edits->offset_sequence_one + 1; ++j)
        outputNode(*(*rbuf_old->buffer)[j], writer);

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

            xmlTextWriterWriteRawLen(writer, content_old, end_old + 1);

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

          }

          if(end_new >= 0) {

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));

            xmlTextWriterWriteRawLen(writer, content_new, end_new + 1);

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

          }

          xmlTextWriterWriteRawLen(writer, content_old + end_old + 1, size_old - (end_old + 1));

          last_diff = edits->offset_sequence_one + edits->length;
          edits = edits->next;
          continue;
        }
      }

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

      for(int j = 0; j < edits->length; ++j)
        outputNode(*(*rbuf_old->buffer)[edits->offset_sequence_one + j], writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));

      for(int j = 0; j < edit_next->length; ++j)
        outputNode(*(*rbuf_new->buffer)[edit_next->offset_sequence_two + j], writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

      last_diff = edits->offset_sequence_one + edits->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:
      //      fprintf(stderr, "HERE\n");
      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));

      for(int j = 0; j < edits->length; ++j)
        outputNode(*(*rbuf_new->buffer)[edits->offset_sequence_two + j], writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

      last_diff = edits->offset_sequence_one + 1;
      break;
    case DELETE:
      //      fprintf(stderr, "HERE\n");

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

      for(int j = 0; j < edits->length; ++j)
        outputNode(*(*rbuf_old->buffer)[edits->offset_sequence_one + j], writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

      last_diff = edits->offset_sequence_one + edits->length;
      break;
    }

  }

  for(unsigned int j = last_diff; j < rbuf_old->buffer->size(); ++j)
    outputNode(*(*rbuf_old->buffer)[j], writer);

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

void update_context(struct reader_buffer * rbuf, xmlTextReaderPtr reader) {

  if(xmlTextReaderIsEmptyElement(reader))
    return;

  xmlNodePtr node = getRealCurrentNode(reader);
  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    rbuf->context->push_back(node);
  } else if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if(rbuf->context->size() == 1)
      return;

    rbuf->context->pop_back();
  }
}

void update_in_diff(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int indiff) {

  if(xmlTextReaderIsEmptyElement(reader))
    return;

  xmlNodePtr node = getRealCurrentNode(reader);
  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    rbuf->in_diff->push_back(indiff);
  } else if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if(rbuf->in_diff->size() == 1)
      return;

    rbuf->in_diff->pop_back();
  }
}

void update_issued_diff(struct reader_buffer * rbuf, xmlTextReaderPtr reader) {

  if(xmlTextReaderIsEmptyElement(reader))
    return;

  xmlNodePtr node = getRealCurrentNode(reader);
  if((xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT) {

    rbuf->issued_diff->push_back(false);
  } else if((xmlReaderTypes)node->type == XML_READER_TYPE_END_ELEMENT) {

    if(rbuf->issued_diff->size() == 1)
      return;

    rbuf->issued_diff->pop_back();
  }
}
