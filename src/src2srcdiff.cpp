/*
  src2srcdiff.cpp

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

// diff node accessor function
const void * node_set_index(int idx, const void *s) {
  std::vector<std::vector<xmlNode *> *> & node_sets = *(std::vector<std::vector<xmlNode *> *> *)s;
  return node_sets[idx];
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

// diff node comparison function
int node_set_compare(const void * e1, const void * e2) {
  std::vector<xmlNode *> * node_set1 = (std::vector<xmlNode *> *)e1;
  std::vector<xmlNode *> * node_set2 = (std::vector<xmlNode *> *)e2;

  if(node_set1->size() != node_set2->size())
    return 1;
  else
    for(int i = 0; i < node_set1->size(); ++i)
      if(node_compare(node_set1->at(i), node_set2->at(i)))
        return 1;

  return 0;
}

bool is_white_space(xmlNodePtr node) {

  if(node->type == XML_READER_TYPE_TEXT) {

    if(isspace((char)node->content[0]))
      return true;
    else
      return false;

  } else
    return false;

}

bool is_text(xmlNodePtr node) {

  if(node->type == XML_READER_TYPE_TEXT)
    return true;
  else
    return false;

}

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2) {
  std::vector<xmlNode *> * node_set1 = (std::vector<xmlNode *> *)e1;
  std::vector<xmlNode *> * node_set2 = (std::vector<xmlNode *> *)e2;

  for(int i = 0, j = 0; i < node_set1->size() && j < node_set2->size(); ++i, ++j) {

    for(; i < node_set1->size() && is_white_space(node_set1->at(i)); ++i);

    for(; j < node_set2->size() && is_white_space(node_set2->at(j)); ++j);

    if(i >= node_set1->size() && j >= node_set2->size())
      return 0;

    if(i >= node_set1->size() || j >= node_set2->size())
      return 1;

    //string consecutive non whitespace text nodes
    if(is_text(node_set1->at(i)) && is_text(node_set2->at(j))) {

      std::string text1 = "";
      for(; i < node_set1->size() && is_text(node_set1->at(i)); ++i)
        if(!is_white_space(node_set1->at(i)))
          text1 += (const char *)node_set1->at(i)->content;

      std::string text2 = "";
      for(; j < node_set2->size() && is_text(node_set2->at(j)); ++j)
        if(!is_white_space(node_set2->at(j)))
          text2 += (const char *)node_set2->at(j)->content;

      if(text1 != text2)
        return 1;

      --i;
      --j;

      continue;

    }

    if(node_compare(node_set1->at(i), node_set2->at(j)))
      return 1;

  }

  return 0;
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
  std::vector<xmlNode *> * diff_nodes;
  std::vector<struct open_diff *> * open_diff;
  std::vector<struct open_diff *> * output_diff;

  std::vector<struct open_diff *> * delay_close;

};

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// compares a line supposed to be the same and output the correrct elements
void compare_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old,struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer);

// create sets of nodes
std::vector<std::vector<xmlNodePtr> *> * create_node_set(std::vector<xmlNodePtr> * diff_nodes, int start, int end);

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int operation, int end_line);

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, struct edit * edit, xmlTextWriterPtr writer);

// output file level info
void output_diffs(struct reader_buffer * rbuf_old, std::vector<std::vector<xmlNodePtr> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<xmlNodePtr> *> * node_sets_new, xmlTextWriterPtr writer);

void output_change(struct reader_buffer * rbuf_old, std::vector<std::vector<xmlNodePtr> *> * node_sets_old
                   , int start_old, int length_old
                   , struct reader_buffer * rbuf_new, std::vector<std::vector<xmlNodePtr> *> * node_sets_new
                   , int start_new, int length_new
                   , xmlTextWriterPtr writer);

void compare_many2many(struct reader_buffer * rbuf_old, std::vector<std::vector<xmlNodePtr> *> * node_sets_old
                       , struct reader_buffer * rbuf_new, std::vector<std::vector<xmlNodePtr> *> * node_sets_new
                       , struct edit * edit_script, xmlTextWriterPtr writer);

void output_handler(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

bool output_peek(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

void update_diff_stack(std::vector<struct open_diff *> * open_diffs, xmlNodePtr node, int operation);

void markup_whitespace(struct reader_buffer * rbuf_old, std::vector<xmlNodePtr> * node_set_old, struct reader_buffer * rbuf_new, std::vector<xmlNodePtr> * node_set_new, xmlTextWriterPtr writer);

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    //    fprintf(stderr, "Usage: src2srcdiff oldFile newFile srcdiffFile\n");
    fprintf(stderr, "Usage: src2srcdiff oldFile newFile dir\n");
    return 1;
  }

  const char * srcdiff_file;
  srcdiff_file = "-";

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

  /*
    int distance = shortest_edit_script(lines1.size(), (void *)&lines1, lines2.size(), (void *)&lines2, line_compare, line_index, &edit_script);

    if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    return distance;
    }
  */

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

    collect_difference(&rbuf_old, reader_old, DELETE, lines1.size() + 1);

    collect_difference(&rbuf_new, reader_new, INSERT, lines2.size() + 1);

    std::vector<std::vector<xmlNodePtr> *> * node_set_old = create_node_set(rbuf_old.diff_nodes, 0, rbuf_old.diff_nodes->size());
    std::vector<std::vector<xmlNodePtr> *> * node_set_new = create_node_set(rbuf_new.diff_nodes, 0, rbuf_new.diff_nodes->size());

    output_diffs(&rbuf_old, node_set_old, &rbuf_new, node_set_new, writer);

    // output srcdiff unit
    outputNode(*getRealCurrentNode(reader_old), writer);

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

  //free_shortest_edit_script(edit_script);

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

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, xmlTextReaderPtr reader, int operation, int end_line) {

  // save beginning of characters
  unsigned char * characters_start = rbuf->characters;

  // allocate new buffer
  rbuf->diff_nodes = new std::vector<xmlNode *>;

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
            rbuf->diff_nodes->push_back(text);

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
          rbuf->diff_nodes->push_back(text);

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
          rbuf->diff_nodes->push_back(text);

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
          rbuf->diff_nodes->push_back(text);

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
      rbuf->diff_nodes->push_back(node);

      not_done = xmlTextReaderRead(reader);
    }

  ++rbuf->line_number;

}

// check if node is whitespace
bool is_whitespace(std::vector<xmlNodePtr> * diff_nodes, int start) {

  if(diff_nodes->at(start)->type != XML_READER_TYPE_TEXT)
    return false;

  if(strspn((const char *)diff_nodes->at(start)->content, " \t\r\n") != strlen((const char *)diff_nodes->at(start)))
    return false;

  return true;


}

// check if node is a indivisable group of three (atomic)
bool is_atomic_srcml(std::vector<xmlNodePtr> * diff_nodes, int start) {

  if((start + 2) >= diff_nodes->size())
    return false;

  if((xmlReaderTypes)diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if((xmlReaderTypes)diff_nodes->at(start + 2)->type != XML_READER_TYPE_END_ELEMENT)
    return false;

  if(strcmp((const char *)diff_nodes->at(start)->name, (const char *)diff_nodes->at(start + 2)->name) != 0)
    return false;

  if(strcmp((const char *)diff_nodes->at(start)->name, "name") == 0)
    return true;

  if(strcmp((const char *)diff_nodes->at(start)->name, "operator") == 0)
    return true;

  if(strcmp((const char *)diff_nodes->at(start)->name, "literal") == 0)
    return true;

  if(strcmp((const char *)diff_nodes->at(start)->name, "modifier") == 0)
    return true;

  return false;
}

// check if sub statement
bool is_sub_statement(struct reader_buffer * rbuf, int start) {

  /*
    C-Preprocessor
    cpp:directive, cpp:file, cpp:include, cpp:define, cpp:undef, cpp:line, cpp:if, cpp:ifdef, cpp:ifndef, cpp:else, cpp:elif, cpp:endif, cpp:then, cpp:pragma, cpp:error
  */
  return false;

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "expr") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "init") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "incr") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "condition") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "name") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "type") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "index") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "decl") == 0)
    return true;

  return false;

}


// check if sub statement
bool is_statement(struct reader_buffer * rbuf, int start) {


  return false;

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "if") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "then") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "else") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "while") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "do") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "switch") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "case") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "default") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "for") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "break") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "continue") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "comment") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "block") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "expr_stmt") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "decl_stmt") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "goto") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "label") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "typedef") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "asm") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "macro") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "enum") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "empty_stmt") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "namespace") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "template") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "using") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "extern") == 0)
    return true;

  return false;

}

bool is_function_related(struct reader_buffer * rbuf, int start) {

  /*
    function, function_decl, specifier, return, call, parameter_list, param, argument_list, argument
  */

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "function") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "function_decl") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "specifier") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "return") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "call") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "parameter_list") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "param") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "argument_list") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "argument") == 0)
    return true;

  return false;
}

bool is_structure_related(struct reader_buffer * rbuf, int start) {

  /*
    struct, struct_decl, union, union_decl
  */

  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "struct") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "struct_decl") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "union") == 0)
    return true;

  if(strcmp((const char *)rbuf->diff_nodes->at(start)->name, "union_decl") == 0)
    return true;

  return false;
}

bool is_preprocessor_related(struct reader_buffer * rbuf, int start) {

  /*
    cpp:directive, cpp:file, cpp:include, cpp:define, cpp:undef, cpp:line,
    cpp:if, cpp:ifdef, cpp:ifndef, cpp:else, cpp:elif, cpp:endif, cpp:then, cpp:pragma, cpp:error
  */
  if((xmlReaderTypes)rbuf->diff_nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;


  return false;

}

void collect_entire_tag(std::vector<xmlNodePtr> * diff_nodes, std::vector <xmlNode *> * node_set, int * start) {

  const char * open_node = (const char *)diff_nodes->at(*start)->name;

  node_set->push_back(diff_nodes->at(*start));

  if(diff_nodes->at(*start)->extra & 0x1)
    return;

  ++(*start);

  std::vector<bool> is_open;

  is_open.push_back(false);

  for(; !is_open.empty(); ++(*start)) {

    node_set->push_back(diff_nodes->at(*start));

    if((xmlReaderTypes)diff_nodes->at(*start)->type == XML_READER_TYPE_ELEMENT
       && !(diff_nodes->at(*start)->extra & 0x1))
      is_open.push_back(false);

    else if((xmlReaderTypes)diff_nodes->at(*start)->type == XML_READER_TYPE_END_ELEMENT)
      is_open.pop_back();

  }

  --(*start);
}

std::vector<std::vector<xmlNodePtr> *> * create_node_set(std::vector<xmlNodePtr> * diff_nodes, int start, int end) {

  std::vector<std::vector<xmlNodePtr> *> * node_sets = new std::vector<std::vector<xmlNodePtr> *>;

  for(int i = start; i < end; ++i) {

    std::vector <xmlNode *> * node_set = new std::vector <xmlNode *>;

    if(is_whitespace(diff_nodes, i) || diff_nodes->at(i)->type == XML_READER_TYPE_TEXT) {

      node_set->push_back(diff_nodes->at(i));

    } else if(is_atomic_srcml(diff_nodes, i)) {

      node_set->push_back(diff_nodes->at(i));
      node_set->push_back(diff_nodes->at(i + 1));
      node_set->push_back(diff_nodes->at(i + 2));

      i += 2;

    } else if((xmlReaderTypes)diff_nodes->at(i)->type == XML_READER_TYPE_ELEMENT) {

      collect_entire_tag(diff_nodes, node_set, &i);

    } else {

      node_set->push_back(diff_nodes->at(i));
    }

    node_sets->push_back(node_set);

  }

  return node_sets;

}

// output a change
void output_diffs(struct reader_buffer * rbuf_old, std::vector<std::vector<xmlNodePtr> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<xmlNodePtr> *> * node_sets_new, xmlTextWriterPtr writer) {

  //fprintf(stderr, "HERE_DOUBLE\n");

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff = 0;
  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    if(rbuf_old->open_diff->back()->operation != COMMON)
      output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;

    // add preceeding unchanged
    if(edits->operation == DELETE)
      for(int j = last_diff; j < edits->offset_sequence_one; ++j)
        for(int i = 0; i < node_sets_old->at(j)->size(); ++i)
          output_handler(rbuf_old, rbuf_new, node_sets_old->at(j)->at(i), COMMON, writer);

    else
      for(int j = last_diff; j < edits->offset_sequence_one + 1; ++j)
        for(int i = 0; i < node_sets_old->at(j)->size(); ++i)
          output_handler(rbuf_old, rbuf_new, node_sets_old->at(j)->at(i), COMMON, writer);

    if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
       && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

      //      fprintf(stderr, "HERE\n");

      // look for pure whitespace change
      int whitespace_length_old = 0;
      int whitespace_length_new = 0;
      if(node_sets_old->at(edits->offset_sequence_one)->at(0)->type == node_sets_new->at(edit_next->offset_sequence_two)->at(0)->type
         && (xmlReaderTypes)node_sets_old->at(edits->offset_sequence_one)->at(0)->type == XML_READER_TYPE_TEXT) {

        strspn((const char *)node_sets_old->at(edits->offset_sequence_one)->at(0)->content, " \t\r\n");
        strspn((const char *)node_sets_new->at(edit_next->offset_sequence_two)->at(0)->content, " \t\r\n");

      }

      if(whitespace_length_old != 0 && whitespace_length_new != 0
         && node_sets_old->at(edits->offset_sequence_one)->size() == 1 && node_sets_new->at(edit_next->offset_sequence_two)->size() == 1
         && edits->length == 1 && edit_next->length == 1) {

        xmlChar * content_old = node_sets_old->at(edits->offset_sequence_one)->at(0)->content;
        xmlChar * content_new = node_sets_new->at(edit_next->offset_sequence_two)->at(0)->content;

        int size_old = strlen((const char *)node_sets_old->at(edits->offset_sequence_one)->at(0)->content);
        int size_new = strlen((const char *)node_sets_new->at(edit_next->offset_sequence_two)->at(0)->content);

        if(whitespace_length_old == size_old && whitespace_length_new == size_new) {

          int end_old = size_old - 1;
          int end_new = size_new - 1;

          while(end_old >= 0 && end_new >= 0 && content_old[end_old] == content_new[end_new]) {

            --end_old;
            --end_new;
          }

          if(end_old >= 0) {

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));

            xmlTextWriterWriteRawLen(writer, content_old, end_old + 1);

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));

          }

          if(end_new >= 0) {

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

            xmlTextWriterWriteRawLen(writer, content_new, end_new + 1);

            // output diff tag
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

          }

          xmlTextWriterWriteRawLen(writer, content_old + end_old + 1, size_old - (end_old + 1));

        }

      } else if(edits->length == edit_next->length && edits->length == 1
                && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
                    || node_sets_old->at(edits->offset_sequence_one)->size() > 1)) {

        if(node_compare(node_sets_old->at(edits->offset_sequence_one)->at(0)
                        , node_sets_new->at(edit_next->offset_sequence_two)->at(0)) == 0
           && node_sets_old->at(edits->offset_sequence_one)->at(0)->type != XML_READER_TYPE_TEXT) {

          if(rbuf_old->open_diff->back()->operation != COMMON)
            output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

          rbuf_old->open_diff->back()->open_tags->front()->marked = false;

          output_handler(rbuf_old, rbuf_new, node_sets_old->at(edits->offset_sequence_one)->at(0), COMMON, writer);

          // collect subset of nodes
          std::vector<std::vector<xmlNodePtr> *> * next_node_set_old = create_node_set(node_sets_old->at(edits->offset_sequence_one), 1
                                                                                       , node_sets_old->at(edits->offset_sequence_one)->size() - 1);
          std::vector<std::vector<xmlNodePtr> *> * next_node_set_new = create_node_set(node_sets_new->at(edit_next->offset_sequence_two), 1
                                                                                       , node_sets_new->at(edit_next->offset_sequence_two)->size() - 1);

          // compare subset of nodes
          output_diffs(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

          output_handler(rbuf_old, rbuf_new,
                         node_sets_old->at(edits->offset_sequence_one)->
                         at(node_sets_old->at(edits->offset_sequence_one)->size() - 1)
                         , COMMON, writer);

          if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
            rbuf_old->open_diff->back()->open_tags->front()->marked = true;

          output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

        } else {

          output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, 1, rbuf_new, node_sets_new, edit_next->offset_sequence_two, 1, writer);

        }

      } else {

        //compare_many2many(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edits, writer);
        output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length
                      , rbuf_new, node_sets_new, edit_next->offset_sequence_two, edit_next->length, writer);

      }

      last_diff = edits->offset_sequence_one + edits->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:

      {

        //      fprintf(stderr, "HERE\n");

        output_change(rbuf_old, node_sets_old, 0, 0, rbuf_new, node_sets_new, edits->offset_sequence_two, edits->length, writer);

        last_diff = edits->offset_sequence_one + 1;

      }

      break;

    case DELETE:

      {

        //fprintf(stderr, "HERE\n");
        output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length, rbuf_new, node_sets_new, 0, 0, writer);

        last_diff = edits->offset_sequence_one + edits->length;
      }

      break;
    }

  }

  // output diff tag start
  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;


  for(int j = last_diff; j < node_sets_old->size(); ++j)
    for(int i = 0; i < node_sets_old->at(j)->size(); ++i)
      output_handler(rbuf_old, rbuf_new, node_sets_old->at(j)->at(i), COMMON, writer);

  if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

  output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

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

    /*
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
             || !rbuf_new->open_diff->back()->open_tags->back()->marked))
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
    */
    if(strcmp((const char *)rbuf->output_diff->back()->open_tags->back()->node->name, (const char *)node->name) != 0)
      return;

      outputNode(*node, writer);

      if(rbuf->output_diff->back()->operation == COMMON) {

        //fprintf(stderr, "HERE OUTPUT COMMON\n");

        update_diff_stack(rbuf_old->open_diff, node, COMMON);
        update_diff_stack(rbuf_new->open_diff, node, COMMON);

        update_diff_stack(rbuf_old->output_diff, node, COMMON);

      } else if(rbuf->output_diff->back()->operation == DELETE) {

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

      //    }

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

struct offset_pair {

  int old_offset;
  int new_offset;
  int similarity;
  struct offset_pair * next;
};

#define MIN -1

int compute_similarity(std::vector<xmlNodePtr> * node_set_old, std::vector<xmlNodePtr> * node_set_new) {

  int length = node_set_new->size();

  if(node_set_syntax_compare(node_set_old, node_set_new) == 0)
    return MIN;
  

  int leftptr;
  for(leftptr = 0; leftptr < node_set_old->size() && leftptr < node_set_new->size() && node_compare(node_set_old->at(leftptr), node_set_new->at(leftptr)) == 0; ++leftptr);

  int rightptr;
  for(rightptr = 1; rightptr <= node_set_old->size() && rightptr <= node_set_new->size()
        && node_compare(node_set_old->at(node_set_old->size() - rightptr), node_set_new->at(node_set_new->size() - rightptr)) == 0; ++rightptr);

  return (node_set_old->size() - rightptr) - leftptr;
}

void match_differences(std::vector<std::vector<xmlNodePtr> *> * node_sets_old
                       , std::vector<std::vector<xmlNodePtr> *> * node_sets_new
                       , struct edit * edit_script, struct offset_pair ** matches) {

  struct edit * edits = edit_script;
  struct edit * edit_next = edit_script->next;

  for(int old_pos = 0, new_pos = 0; old_pos < edits->length && new_pos < edit_next->length; ++old_pos, ++new_pos) {

    // TODO: set to first or positive infinity or MAX_INT or whatever it is called
    int min_similarity = 1000;
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

void compare_many2many(struct reader_buffer * rbuf_old, std::vector<std::vector<xmlNodePtr> *> * node_sets_old
                       , struct reader_buffer * rbuf_new, std::vector<std::vector<xmlNodePtr> *> * node_sets_new
                       , struct edit * edit_script, xmlTextWriterPtr writer) {

  struct edit * edits = edit_script;
  struct edit * edit_next = edit_script->next;

  struct offset_pair * matches;

  match_differences(node_sets_old, node_sets_new, edit_script, &matches);

  int last_old = 0;
  int last_new = 0;

  // TODO:  not yet handling different sizes
  for(; matches; matches = matches->next) {

    // output diffs until match
    output_change(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old, matches->old_offset - last_old,
                  rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new, matches->new_offset - last_new, writer);

    // correct could only be whitespace
    if(matches->similarity == MIN) {

      if(rbuf_old->open_diff->back()->operation != COMMON)
        output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

      rbuf_old->open_diff->back()->open_tags->front()->marked = false;

      markup_whitespace(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)
                        , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset), writer);

      if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
        rbuf_old->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

    }

    else {

      output_change(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset, 1,
                    rbuf_new, node_sets_new, edit_next->offset_sequence_two + matches->new_offset, 1, writer);

    }

    last_old = matches->old_offset + 1;
    last_new = matches->new_offset + 2;

  }

  // output diffs until match
  output_change(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old, (edits->length - last_old),
                rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new, (edit_next->length - last_new), writer);


}

bool output_peek(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer) {

  //fprintf(stderr, "HERE PEAK\n");

  return true;

}


void markup_whitespace(struct reader_buffer * rbuf_old, std::vector<xmlNodePtr> * node_set_old, struct reader_buffer * rbuf_new, std::vector<xmlNodePtr> * node_set_new, xmlTextWriterPtr writer) {

  for(int i = 0, j = 0; i < node_set_old->size(), j < node_set_new->size(); ++i, ++j) {

    if(node_compare(node_set_old->at(i), node_set_new->at(j)) == 0)

      output_handler(rbuf_old, rbuf_new, node_set_old->at(i), COMMON, writer);

    else if(is_white_space(node_set_old->at(i)) && is_white_space(node_set_new->at(j))) {

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));

      output_handler(rbuf_old, rbuf_new, node_set_old->at(i), DELETE, writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

      output_handler(rbuf_old, rbuf_new, node_set_new->at(j), INSERT, writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

      // whitespace change
    } else if(is_white_space(node_set_old->at(i))) {

      // whitespace delete
      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));

      output_handler(rbuf_old, rbuf_new, node_set_old->at(i), DELETE, writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));

      --j;

    } else if(is_white_space(node_set_new->at(j))) {

      //whitespace insert
      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

      output_handler(rbuf_old, rbuf_new, node_set_new->at(j), INSERT, writer);

      // output diff tag
      xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

      --i;

    } else {

      // handle () and ( ) first is one text node, other is three
      if(strlen((const char *)node_set_old->at(i)->content) < strlen((const char *)node_set_new->at(j)->content)) {

        output_handler(rbuf_old, rbuf_new, node_set_old->at(i), COMMON, writer);

        ++i;

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old>"));

        output_handler(rbuf_old, rbuf_new, node_set_old->at(i), DELETE, writer);

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));

        ++i;

        output_handler(rbuf_old, rbuf_new, node_set_old->at(i), COMMON, writer);

      } else {

        output_handler(rbuf_old, rbuf_new, node_set_new->at(j), COMMON, writer);

        ++j;

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new>"));

        output_handler(rbuf_old, rbuf_new, node_set_new->at(j), INSERT, writer);

        // output diff tag
        xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

        ++j;

        output_handler(rbuf_old, rbuf_new, node_set_new->at(j), COMMON, writer);

      }

    }

  }

}

void output_change(struct reader_buffer * rbuf_old, std::vector<std::vector<xmlNodePtr> *> * node_sets_old
                   , int start_old, int length_old
                   , struct reader_buffer * rbuf_new, std::vector<std::vector<xmlNodePtr> *> * node_sets_new
                   , int start_new, int length_new
                   , xmlTextWriterPtr writer) {

  if(length_old > 0) {

    // output diff tag start
    if(rbuf_old->open_diff->back()->operation != DELETE)
      output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;

    for(int j = 0; j < length_old; ++j)
      for(int i = 0; i < node_sets_old->at(start_old + j)->size(); ++i)
        output_handler(rbuf_old, rbuf_new, node_sets_old->at(start_old + j)->at(i), DELETE, writer);

    // output diff tag start
    if(rbuf_old->open_diff->back()->operation == DELETE)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

  }

  if(length_new > 0) {

    // output diff tag
    if(rbuf_new->open_diff->back()->operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

    rbuf_new->open_diff->back()->open_tags->front()->marked = false;

    for(int j = 0; j < length_new; ++j)
      for(int i = 0; i < node_sets_new->at(start_new + j)->size(); ++i)
        output_handler(rbuf_old, rbuf_new, node_sets_new->at(start_new + j)->at(i), INSERT, writer);

    // output diff tag start
    if(rbuf_new->open_diff->back()->operation == INSERT)
      rbuf_new->open_diff->back()->open_tags->front()->marked = true;
    output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);

  }

}
