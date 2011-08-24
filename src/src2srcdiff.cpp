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

#include "xmlrw.h"
#include "diffrw.h"

// macros
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

// constant template for temporary file names
char * srcdiff_template = (char *)"srcdifftemp.XXX";
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

// converts source code to srcML
void translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

// stores information during xml Text Reader processing
struct reader_buffer {

  int line_number;
  unsigned char * characters;
  std::vector<xmlNode *> * buffer;
};

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// outputs a line of xml (used for old file)
void output_xml_line(struct reader_buffer * rbuf, xmlTextReaderPtr reader, xmlTextWriterPtr writer);

// advances to next line of xml (used for new file)
void next_xml_line(struct reader_buffer * rbuf, xmlTextReaderPtr reader);

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, struct edit * edit);

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf, struct edit * edit, xmlTextWriterPtr writer);

// output a change
void output_double(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new,  struct edit * edit, xmlTextWriterPtr writer);

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    //    fprintf(stderr, "Usage: src2srcdiff oldFile newFile srcdiffFile\n");
    fprintf(stderr, "Usage: src2srcdiff oldFile newFile dir\n");
    return 1;
  }

  const char * srcdiff_file;
  srcdiff_file = "/dev/stdout";
  /*
    if(argc < 4) {

    srcdiff_file = "/dev/stdout";
    }
    else {

    if(strcmp(argv[1], argv[3]) == 0 || strcmp(argv[2], argv[3]) == 0) {

    fprintf(stderr, "Input and output file must be different\n");
    return 1;
    }

    srcdiff_file = argv[3];
    }
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

    // run through diffs adding markup
    int last_diff = 0;
    struct reader_buffer rbuf_old = { NULL };
    xmlTextReaderRead(reader_old);

    struct reader_buffer rbuf_new = { NULL };
    xmlTextReaderRead(reader_new);

    // create srcdiff unit
    xmlNodePtr unit = create_srcdiff_unit(reader_old, reader_new);

    // output srcdiff unit
    outputNode(*unit, writer);

    xmlTextReaderRead(reader_old);
    xmlTextReaderRead(reader_new);

    /*    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<unit xmlns=\"http://www.sdml.info/srcML/src\" xmlns:cpp=\"http://www.sdml.info/srcML/cpp\" xmlns:diff=\"http://www.sdml.info/srcML/srcDiff\" language=\"C\" filename=\""));
          xmlTextWriterWriteRawLen(writer, BAD_CAST argv[1], strlen(argv[1]));
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("\">"));
    */
    struct edit * edits = edit_script;
    for (; edits; edits = edits->next) {

      // add preceeding unchanged
      if(edits->operation == DELETE)
        for(int j = last_diff; j < edits->offset_sequence_one; ++rbuf_old.line_number, ++j)
          output_xml_line(&rbuf_old, reader_old, writer);
      else
        for(int j = last_diff; j < edits->offset_sequence_one + 1; ++rbuf_old.line_number, ++j)
          output_xml_line(&rbuf_old, reader_old, writer);

      // detect and change
      struct edit * edit_next = edits->next;
      if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
         && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

        collect_difference(&rbuf_old, reader_old, edits);

        for(; rbuf_new.line_number < edits->next->offset_sequence_two; ++rbuf_new.line_number)
          next_xml_line(&rbuf_new, reader_new);

        collect_difference(&rbuf_new, reader_new, edits->next);

        output_double(&rbuf_old, &rbuf_new, edits, writer);

        last_diff = edits->offset_sequence_one + edits->length;
        edits = edits->next;
        continue;
      }

      // handle pure delete or insert
      switch (edits->operation) {

      case INSERT:
        for(; rbuf_new.line_number < edits->offset_sequence_two; ++rbuf_new.line_number)
          next_xml_line(&rbuf_new, reader_new);

        collect_difference(&rbuf_new, reader_new, edits);
        output_single(&rbuf_new, edits, writer);
        //for(int j = 0; j < edits->length; ++j)
        //output_xml_line(&rbuf_new, reader_new, writer);
        last_diff = edits->offset_sequence_one + 1;
        break;
      case DELETE:
        //        for(int j = rbuf_old.line_number - edits->offset_sequence_one; j < edits->length; ++rbuf_old.line_number, ++j)
        //output_xml_line(&rbuf_old, reader_old, writer);

        collect_difference(&rbuf_old, reader_old, edits);
        output_single(&rbuf_old, edits, writer);
        //for(int i = 0; i < rbuf_old.buffer->size(); ++i)
        //outputNode(*(*rbuf_old.buffer)[i], writer);

        last_diff = edits->offset_sequence_one + edits->length;
        break;
      }

    }

    for(unsigned int j = last_diff; j < lines1.size(); ++rbuf_old.line_number, ++j)
      output_xml_line(&rbuf_old, reader_old, writer);

  }

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
  OPTION_TYPE options = OPTION_CPP_MARKUP_ELSE | OPTION_CPP | OPTION_XMLDECL | OPTION_XML;

  // create translator object
  srcMLTranslator translator(language, srcml_file, options);

  // set input file (must be done)
  translator.setInput(source_file);

  // translate file
  translator.translate(NULL, dir, NULL, NULL, language);

  // close the input file
  translator.close();
}

// outputs a line of xml (used for old file)
void output_xml_line(struct reader_buffer * rbuf, xmlTextReaderPtr reader, xmlTextWriterPtr writer) {

  while(true)

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty
      if(!rbuf->characters)
        rbuf->characters = (unsigned char *)xmlTextReaderConstValue(reader);

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

        // increase new line count and exit
        if((*rbuf->characters) == '\n') {

          ++rbuf->characters;
          return;
        }
      }

      // end text node if finished and get next node
      if(!(*rbuf->characters)) {

        rbuf->characters = NULL;

        xmlTextReaderRead(reader);
      }
    }
    else {

      // output non-text node and get next node
      outputXML(reader, writer);
      xmlTextReaderRead(reader);
    }
}

// advances to next line of xml (used for new file)
void next_xml_line(struct reader_buffer * rbuf, xmlTextReaderPtr reader) {

  while(true)

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty
      if(!rbuf->characters)
        rbuf->characters = (unsigned char *)xmlTextReaderConstValue(reader);

      // cycle through characters
      for (; *rbuf->characters != 0; ++rbuf->characters) {

        // increase new line count and exit
        if((*rbuf->characters) == '\n') {

          ++rbuf->characters;
          return;
        }
      }

      // end text node if finished and get next node
      if(!(*rbuf->characters)) {

        rbuf->characters = NULL;

        xmlTextReaderRead(reader);
      }
    }
    else

      // get next node
      xmlTextReaderRead(reader);
}

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, struct edit * edit) {

  // save beginning of characters
  unsigned char * characters_start = rbuf->characters;

  // allocate new buffer
  rbuf->buffer = new std::vector<xmlNode *>;
  while(true)

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty and set start of characters
      if(!rbuf->characters) {

        characters_start = (unsigned char *)xmlTextReaderConstValue(reader);
        rbuf->characters = characters_start;
      }

      // cycle through characters
      for (; *rbuf->characters != 0; ++rbuf->characters) {

        // increase new line count and check if end of diff
        if((*rbuf->characters) == '\n') {

          ++rbuf->characters;
          ++rbuf->line_number;

          // check if end of diff and create text node for text fragment
          if(rbuf->line_number == (edit->operation == DELETE ? edit->offset_sequence_one : edit->offset_sequence_two) + edit->length) {

            xmlNode * text = new xmlNode;
            text->type = (xmlElementType)XML_READER_TYPE_TEXT;
            text->name = (const xmlChar *)"text";


            const char * content = strndup((const char *)characters_start, (rbuf->characters - characters_start) - 1);
            text->content = (xmlChar *)content;
            rbuf->buffer->push_back(text);
            return;
          }

        }
      }


      // end and save text node if finished and get next node
      if(!(*rbuf->characters)) {

        // create new node and buffer it
        xmlNode * text = new xmlNode;
        text->type = (xmlElementType)XML_READER_TYPE_TEXT;
        text->name = (const xmlChar *)"text";

        const char * content = strdup((const char *)characters_start);
        text->content = (xmlChar *)content;
        rbuf->buffer->push_back(text);

        rbuf->characters = NULL;

        xmlTextReaderRead(reader);
      }
    }
    else {

      // save non-text node and get next node
      rbuf->buffer->push_back(getCurrentNode(reader));

      xmlTextReaderRead(reader);
    }

}

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf, struct edit * edit, xmlTextWriterPtr writer) {

  // for each element start from beginning
  unsigned int i;
  for(i = 0; i < rbuf->buffer->size(); ++i) {

    // skip non-start element
    if((xmlReaderTypes)(*rbuf->buffer)[i]->type != XML_READER_TYPE_ELEMENT)
      continue;

    // for each end element from end
    for(unsigned int j = i + 1; j < rbuf->buffer->size(); ++j) {

      // skip non-end elements
      if((xmlReaderTypes)(*rbuf->buffer)[j]->type != XML_READER_TYPE_END_ELEMENT)
        continue;

      // test if matching tags
      if(strcmp((const char *)(*rbuf->buffer)[i]->name, (const char *)(*rbuf->buffer)[j]->name) == 0) {

        // output elements before matching tags
        for(unsigned int k = 0; k < i; ++k)
          outputNode(*(*rbuf->buffer)[k], writer);


        // output correct diff tag and on delete output elements before matching tag and diff element
        if(edit->operation == DELETE) {

          // output diff tag start
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
        }
        else {

          // output diff tag start
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));
        }

        // output matching tags
        for(unsigned int k = i; k <= j; ++k)
          outputNode(*(*rbuf->buffer)[k], writer);

        // output diff tags and elements after match on delete
        if(edit->operation == DELETE) {

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

        }
        else {

          // output diff tag
          xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

        }

        // output elements after match
        for(unsigned int k = j + 1; k < rbuf->buffer->size(); ++k)
          outputNode(*(*rbuf->buffer)[k], writer);

        // output newline
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("\n"));

        // exit after match
        return;
      }
    }
  }

  // output correct diff tag and on delete output elements before matching tag and diff element
  if(edit->operation == DELETE) {

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));
  }
  else {

    // output diff tag start
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));
  }


  // if did not match then probably closing open tags from previous diff
  for(i = i - 2; i > 0; --i)
    // find non-end elements
    if((xmlReaderTypes)(*rbuf->buffer)[i]->type != XML_READER_TYPE_END_ELEMENT)
      break;

  // output diff
  for(unsigned int j = 0; j <= i ; ++j)
    outputNode(*(*rbuf->buffer)[j], writer);

  // output diff tags and elements after match on delete
  if(edit->operation == DELETE) {

    // output diff tag
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

  }
  else {

    // output diff tag
    xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

  }

  // output remaining
  for(unsigned int j = i + 1; j < rbuf->buffer->size() ; ++j)
    outputNode(*(*rbuf->buffer)[j], writer);
}

// output a change
void output_double(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new,  struct edit * edit, xmlTextWriterPtr writer) {

  // move left change pointer
  unsigned int start = 0;
  for(; start < rbuf_old->buffer->size() && start < rbuf_new->buffer->size() && strcmp((const char *)(*rbuf_old->buffer)[start]->name, (const char *)(*rbuf_new->buffer)[start]->name) == 0; ++start)

    // end if text node contents differ
    if((xmlReaderTypes)(*rbuf_old->buffer)[start]->type == XML_READER_TYPE_TEXT && strcmp((const char *)(*rbuf_old->buffer)[start]->content, (const char *)(*rbuf_new->buffer)[start]->content) != 0)
      break;

  // detect if no change may be error
  if(start == rbuf_old->buffer->size() || start == rbuf_new->buffer->size()) {

    fprintf(stderr, "ERROR\n");
    return;
  }

  // move right change pointer
  unsigned int end = 1;
  for(; (rbuf_old->buffer->size() - end) > 0 && (rbuf_new->buffer->size() - end) > 0 && strcmp((const char *)(*rbuf_old->buffer)[rbuf_old->buffer->size() - end]->name, (const char *)(*rbuf_new->buffer)[rbuf_new->buffer->size() - end]->name) == 0; ++end)

    // end if text node contents differ
    if((xmlReaderTypes)(*rbuf_old->buffer)[rbuf_old->buffer->size() - end]->type == XML_READER_TYPE_TEXT && strcmp((const char *)(*rbuf_old->buffer)[rbuf_old->buffer->size() - end]->content, (const char *)(*rbuf_new->buffer)[rbuf_new->buffer->size() - end]->content) != 0)
      break;

  // detect if no change may be error
  if((rbuf_old->buffer->size() - end) == 0 || (rbuf_new->buffer->size() - end) == 0) {

    fprintf(stderr, "ERROR\n");
    return;
  }

  // output preceeding nodes from old
  for(unsigned int i = 0; i < start; ++i)
    outputNode(*(*rbuf_old->buffer)[i], writer);

  // output diff tag
  xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"start\"/>"));

  // output deleted nodes
  for(unsigned int i = start; i <= rbuf_old->buffer->size() - end; ++i)
    outputNode(*(*rbuf_old->buffer)[i], writer);

  // output diff tag
  xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old status=\"end\"/>"));

  // output diff tag
  xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"start\"/>"));

  // output inserted nodes
  for(unsigned int i = start; i <= rbuf_new->buffer->size() - end; ++i)
    outputNode(*(*rbuf_new->buffer)[i], writer);

  // output diff tag
  xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new status=\"end\"/>"));

  // output nodes after change from old
  for(unsigned int i = (rbuf_old->buffer->size() - end) + 1; i < rbuf_old->buffer->size(); ++i)
    outputNode(*(*rbuf_old->buffer)[i], writer);

  xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("\n"));
}

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new) {

  // get units from source code
  xmlNodePtr unit = getCurrentNode(reader_old);
  xmlNodePtr unit_new = getCurrentNode(reader_old);

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
