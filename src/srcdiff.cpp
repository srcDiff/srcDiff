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

xmlNs diff = { 0, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", 0 };

// global structures
std::vector<xmlNode *> nodes_old;
std::vector<xmlNode *> nodes_new;


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

bool attribute_compare(xmlAttrPtr attr1, xmlAttrPtr attr2) {

  xmlAttrPtr attr_old = attr1;
  xmlAttrPtr attr_new = attr2;

  for(; attr_old && attr_new
        && strcmp((const char *)attr_old->name, (const char *)attr_new->name) == 0
        && strcmp((const char *)attr_old->children->content, (const char *)attr_new->children->content) == 0;
      attr_old = attr_old->next, attr_new = attr_new->next)
    ;

  if(attr_old || attr_new)
    return 1;

  return 0;

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
      return attribute_compare(node1->properties, node2->properties);
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
    for(unsigned int i = 0; i < node_set1->size(); ++i)
      if(node_compare(node_set1->at(i), node_set2->at(i)))
        return 1;

  return 0;
}

bool is_white_space(xmlNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

bool is_white_space_set(std::vector<xmlNodePtr> * node_set) {

  for(unsigned int i = 0; i < node_set->size(); ++i)
    if((xmlReaderTypes)node_set->at(i)->type != XML_READER_TYPE_TEXT || !isspace((char)node_set->at(i)->content[0]))
      return false;

  return true;

}

bool is_text(xmlNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT;
}

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2) {
  std::vector<xmlNode *> * node_set1 = (std::vector<xmlNode *> *)e1;
  std::vector<xmlNode *> * node_set2 = (std::vector<xmlNode *> *)e2;

  for(unsigned int i = 0, j = 0; i < node_set1->size() && j < node_set2->size(); ++i, ++j) {

    // Bypassing whitespace
    for(; i < node_set1->size() && is_white_space(node_set1->at(i)); ++i)
      ;

    // Bypassing whitespace
    for(; j < node_set2->size() && is_white_space(node_set2->at(j)); ++j)
      ;

    // If end was all whitespace then the same
    if(i >= node_set1->size() && j >= node_set2->size())
      return 0;

    // If one had ending whitespace and other had something else then different
    if(i >= node_set1->size() || j >= node_set2->size())
      return 1;

    // string consecutive non whitespace text nodes
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

// diff node comparison function
int node_set_comment_compare(const void * e1, const void * e2) {
  std::vector<xmlNode *> * node_set1 = (std::vector<xmlNode *> *)e1;
  std::vector<xmlNode *> * node_set2 = (std::vector<xmlNode *> *)e2;

  if(is_white_space_set(node_set1) && is_white_space_set(node_set2))
    return node_set_compare(node_set1, node_set2);

  for(unsigned int i = 0, j = 0; i < node_set1->size() && j < node_set2->size(); ++i, ++j) {

    // Bypassing whitespace
    for(; i < node_set1->size() && is_white_space(node_set1->at(i)); ++i)
      ;

    // Bypassing whitespace
    for(; j < node_set2->size() && is_white_space(node_set2->at(j)); ++j)
      ;

    // If end was all whitespace then the same
    if(i >= node_set1->size() && j >= node_set2->size())
      return 0;

    // If one had ending whitespace and other had something else then different
    if(i >= node_set1->size() || j >= node_set2->size())
      return 1;

    // string consecutive non whitespace text nodes
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
xmlBuffer* translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

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
  std::vector<struct open_diff *> * open_diff;
  std::vector<struct open_diff *> * output_diff;

  std::vector<struct open_diff *> * delay_close;

};

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// compares a line supposed to be the same and output the correrct elements
void compare_same_line(struct reader_buffer * rbuf_old, xmlTextReaderPtr reader_old,struct reader_buffer * rbuf_new, xmlTextReaderPtr reader_new, xmlTextWriterPtr writer);

// create sets of nodes
std::vector<std::vector<int> *> * create_node_set(std::vector<xmlNodePtr> * nodes, int start, int end);

// collect the differnces
void collect_difference(struct reader_buffer * rbuf, std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader, int reader_state);

// output a single difference DELETE or INSERT
void output_single(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, struct edit * edit, xmlTextWriterPtr writer);

// output file level info
void output_diffs(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer);

void output_comment_paragraph(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer);

void output_comment_line(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer);

void output_recursive(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old
                      , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new
                      , xmlTextWriterPtr writer);

void output_change(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                   , int start_old, int length_old
                   , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                   , int start_new, int length_new
                   , xmlTextWriterPtr writer);

void compare_many2many(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , struct edit * edit_script, xmlTextWriterPtr writer);

void output_handler(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

bool output_peek(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

void update_diff_stack(std::vector<struct open_diff *> * open_diffs, xmlNodePtr node, int operation);

void markup_whitespace(struct reader_buffer * rbuf_old, std::vector<int> * node_set_old, struct reader_buffer * rbuf_new, std::vector<int> * node_set_new, xmlTextWriterPtr writer);

void output_char(char character, xmlTextWriterPtr writer);


int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    fprintf(stderr, "Usage: srcdiff oldFile newFile dir\n");
    return 1;
  }

  const char * srcdiff_file;
  srcdiff_file = "-";

  diff_common_start = new xmlNode;
  diff_common_start->name = (xmlChar *) DIFF_COMMON;
  diff_common_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_common_start->extra = 0;

  diff_common_end = new xmlNode;
  diff_common_end->name = (xmlChar *) DIFF_COMMON;
  diff_common_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_common_end->extra = 0;

  diff_old_start = new xmlNode;
  diff_old_start->name = (xmlChar *) DIFF_OLD;
  diff_old_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_old_start->extra = 0;

  diff_old_end = new xmlNode;
  diff_old_end->name = (xmlChar *) DIFF_OLD;
  diff_old_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_old_end->extra = 0;

  diff_new_start = new xmlNode;
  diff_new_start->name = (xmlChar *) DIFF_NEW;
  diff_new_start->type = (xmlElementType)XML_READER_TYPE_ELEMENT;
  diff_new_start->extra = 0;

  diff_new_end = new xmlNode;
  diff_new_end->name = (xmlChar *) DIFF_NEW;
  diff_new_end->type = (xmlElementType)XML_READER_TYPE_END_ELEMENT;
  diff_new_end->extra = 0;

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
  {
    // create the reader for the old file
    reader_old = xmlReaderForMemory((const char*) xmlBufferContent(output_file_one), output_file_one->use, 0, 0, 0);
    if (reader_old == NULL) {

      goto cleanup;
    }

    // create the reader for the new file
    reader_new = xmlReaderForMemory((const char*) xmlBufferContent(output_file_two), output_file_two->use, 0, 0, 0);
    //    reader_new = xmlNewTextReader(xmlBufferContent(output_file_two));
    //    reader_new = xmlNewTextReaderFilename(srcml_file_two);
    if (reader_new == NULL) {

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
    struct reader_buffer rbuf_old = { 0 };
    rbuf_old.stream_source = DELETE;
    rbuf_old.open_diff = new std::vector<struct open_diff *>;

    new_diff = new struct open_diff;
    new_diff->operation = COMMON;
    new_diff->open_tags = new std::vector<struct tag *>;
    rbuf_old.open_diff->push_back(new_diff);

    rbuf_old.output_diff = &output_diff;
    rbuf_old.delay_close = new std::vector<struct open_diff *>;
    xmlTextReaderRead(reader_old);

    struct reader_buffer rbuf_new = { 0 };
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

    int is_old = xmlTextReaderRead(reader_old);
    int is_new = xmlTextReaderRead(reader_new);

    collect_difference(&rbuf_old, &nodes_old, reader_old, is_old);

    xmlBufferFree(output_file_one);

    collect_difference(&rbuf_new, &nodes_new, reader_new, is_new);

    xmlBufferFree(output_file_two);

    std::vector<std::vector<int> *> * node_set_old = create_node_set(&nodes_old, 0, nodes_old.size());
    std::vector<std::vector<int> *> * node_set_new = create_node_set(&nodes_new, 0, nodes_new.size());

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

  return status;
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
void collect_difference(struct reader_buffer * rbuf, std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader, int reader_state) {

  // save beginning of characters
  unsigned char * characters_start = rbuf->characters;

  if(!reader_state)
    return;

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
            nodes->push_back(text);

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
          nodes->push_back(text);
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
          nodes->push_back(text);
          characters_start = rbuf->characters + 1;

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
          nodes->push_back(text);

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
      nodes->push_back(node);

      not_done = xmlTextReaderRead(reader);
    }

  ++rbuf->line_number;

}

// check if node is whitespace
bool is_white_space(std::vector<xmlNodePtr> * diff_nodes, int start) {

  if((xmlReaderTypes)diff_nodes->at(start)->type != XML_READER_TYPE_TEXT)
    return false;

  if(strspn((const char *)diff_nodes->at(start)->content, " \t\r\n") != strlen((const char *)diff_nodes->at(start)))
    return false;

  return true;


}

// check if node is a indivisable group of three (atomic)
bool is_atomic_srcml(std::vector<xmlNodePtr> * diff_nodes, unsigned start) {

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

void collect_entire_tag(std::vector<xmlNodePtr> * nodes, std::vector<int> * node_set, int * start) {

  //const char * open_node = (const char *)nodes->at(*start)->name;

  node_set->push_back(*start);

  if(nodes->at(*start)->extra & 0x1)
    return;

  ++(*start);

  std::vector<bool> is_open;

  is_open.push_back(false);

  for(; !is_open.empty(); ++(*start)) {

    node_set->push_back(*start);

    if((xmlReaderTypes)nodes->at(*start)->type == XML_READER_TYPE_ELEMENT
       && !(nodes->at(*start)->extra & 0x1))
      is_open.push_back(false);

    else if((xmlReaderTypes)nodes->at(*start)->type == XML_READER_TYPE_END_ELEMENT)
      is_open.pop_back();

  }

  --(*start);
}

std::vector<std::vector<int> *> * create_node_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  std::vector<std::vector<int> *> * node_sets = new std::vector<std::vector<int> *>;

  for(int i = start; i < end; ++i) {

    std::vector <int> * node_set = new std::vector <int>;

    if(is_white_space(nodes, i))

      node_set->push_back(i);

    else if((xmlReaderTypes)nodes->at(i)->type == XML_READER_TYPE_TEXT) {

      node_set->push_back(i);

    } else if(is_atomic_srcml(nodes, i)) {

      node_set->push_back(i);
      node_set->push_back(i + 1);
      node_set->push_back(i + 2);

      i += 2;

    } else if((xmlReaderTypes)nodes->at(i)->type == XML_READER_TYPE_ELEMENT) {

      collect_entire_tag(nodes, node_set, &i);

    } else {

      node_set->push_back(i);
    }

    node_sets->push_back(node_set);

  }

  return node_sets;

}

// output a change
void output_diffs(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer) {

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
  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    if(rbuf_old->open_diff->back()->operation != COMMON)
      output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;

    // add preceeding unchanged
    if(edits->operation == DELETE)
      for(int j = last_diff_old, k = last_diff_new; j < edits->offset_sequence_one; ++j, ++k)
        markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);

    else
      for(int j = last_diff_old, k = last_diff_new; j < edits->offset_sequence_one + 1; ++j, ++k)
        markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);
 
    if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
       && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

      //      fprintf(stderr, "HERE\n");

      // TODO:  maybe replace with markup_whitespace
      // look for pure whitespace change
      int whitespace_length_old = 0;
      int whitespace_length_new = 0;

      
      if(nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type == nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))->type
         && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type == XML_READER_TYPE_TEXT) {

        strspn((const char *)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->content, " \t\r\n");
        strspn((const char *)nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))->content, " \t\r\n");

      }

      if(whitespace_length_old != 0 && whitespace_length_new != 0
         && node_sets_old->at(edits->offset_sequence_one)->size() == 1 && node_sets_new->at(edit_next->offset_sequence_two)->size() == 1
         && edits->length == 1 && edit_next->length == 1) {

        xmlChar * content_old = nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->content;
        xmlChar * content_new = nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))->content;

        int size_old = strlen((const char *)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->content);
        int size_new = strlen((const char *)nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))->content);

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

        if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))
                        , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))) == 0
           && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type != XML_READER_TYPE_TEXT) {

          output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one
                           , rbuf_new, node_sets_new, edit_next->offset_sequence_two, writer);

        } else {

          output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, 1, rbuf_new, node_sets_new, edit_next->offset_sequence_two, 1, writer);

        }

      } else {

        compare_many2many(rbuf_old, node_sets_old, rbuf_new, node_sets_new, edits, writer);

      }

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old, 0, 0, rbuf_new, node_sets_new, edits->offset_sequence_two, edits->length, writer);

      last_diff_old = edits->offset_sequence_one + 1;
      last_diff_new = edits->offset_sequence_two + edits->length;

      break;

    case DELETE:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length, rbuf_new, node_sets_new, 0, 0, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edits->offset_sequence_two + 1;

      break;
    }

  }

  // output diff tag start
  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;


  for(unsigned int j = last_diff_old, k = last_diff_new; j < node_sets_old->size(); ++j, ++k)
    markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);

  if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

  output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

  free_shortest_edit_script(edit_script);

}

bool contains_new_line(xmlNodePtr node) {

  unsigned int length = strlen((const char *)node->content);

  for(unsigned int i = 0; i < length; ++i)
    if(node->content[i] == '\n')
      return true;

  return false;

}

std::vector<std::vector<int> *> * create_comment_paragraph_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  std::vector<std::vector<int> *> * node_sets = new std::vector<std::vector<int> *>;

  for(int i = start; i < end; ++i) {

    std::vector <int> * node_set = new std::vector <int>;

    if(contains_new_line(nodes->at(i))) {

      for(; contains_new_line(nodes->at(i)); ++i)
        node_set->push_back(i);

      --i;

    } else {

      bool first_newline = false;
      for(; i < end; ++i) {

        if(first_newline && contains_new_line(nodes->at(i))) {

          --i;
          break;

        } else
          first_newline = false;

        node_set->push_back(i);

        if(!first_newline && contains_new_line(nodes->at(i)))
          first_newline = true;

      }

    }

    node_sets->push_back(node_set);

  }

  return node_sets;

}

std::vector<std::vector<int> *> * create_comment_line_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  std::vector<std::vector<int> *> * node_sets = new std::vector<std::vector<int> *>;

  for(int i = start; i < end; ++i) {

    std::vector<int> * node_set = new std::vector <int>;

    for(; i < end; ++i) {

      node_set->push_back(i);

      if(contains_new_line(nodes->at(i)))
        break;
    }

    node_sets->push_back(node_set);

  }

  return node_sets;

}

void output_comment_paragraph(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer) {

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_comment_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    if(rbuf_old->open_diff->back()->operation != COMMON)
      output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;

    // add preceeding unchanged
    if(edits->operation == DELETE)
      for(int j = last_diff_old, k = last_diff_new; j < edits->offset_sequence_one; ++j, ++k)
        markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);

    else
      for(int j = last_diff_old, k = last_diff_new; j < edits->offset_sequence_one + 1; ++j, ++k)
        markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);


    if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
       && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> * next_node_set_old
          = create_comment_line_set(&nodes_old, node_sets_old->at(edits->offset_sequence_one)->at(0)
                  , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1));

        std::vector<std::vector<int> *> * next_node_set_new
          = create_comment_line_set(&nodes_new, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                  , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1));

        output_comment_line(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

      } else
        output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length
                      , rbuf_new, node_sets_new, edit_next->offset_sequence_two, edit_next->length, writer);


      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old, 0, 0, rbuf_new, node_sets_new, edits->offset_sequence_two, edits->length, writer);

      last_diff_old = edits->offset_sequence_one + 1;
      last_diff_new = edits->offset_sequence_two + edits->length;

      break;

    case DELETE:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length, rbuf_new, node_sets_new, 0, 0, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edits->offset_sequence_two + 1;

      break;
    }

  }

  // output diff tag start
  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;


  for(unsigned int j = last_diff_old, k = last_diff_new; j < node_sets_old->size(); ++j, ++k)
    markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);

  if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

  output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

  free_shortest_edit_script(edit_script);

}


void output_comment_line(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer) {

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    if(rbuf_old->open_diff->back()->operation != COMMON)
      output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;

    // add preceeding unchanged
    if(edits->operation == DELETE)
      for(int j = last_diff_old, k = last_diff_new; j < edits->offset_sequence_one; ++j, ++k)
        markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);

    else
      for(int j = last_diff_old, k = last_diff_new; j < edits->offset_sequence_one + 1; ++j, ++k)
        markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);


    if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(edits->operation == DELETE && edits->next != NULL && edit_next->operation == INSERT
       && (edits->offset_sequence_one + edits->length - 1) == edits->next->offset_sequence_one) {

      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> * next_node_set_old
          = create_node_set(&nodes_old, node_sets_old->at(edits->offset_sequence_one)->at(0)
                  , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1));

        std::vector<std::vector<int> *> * next_node_set_new
          = create_node_set(&nodes_new, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                  , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1));

        output_diffs(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

      } else
        output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length
                      , rbuf_new, node_sets_new, edit_next->offset_sequence_two, edit_next->length, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old, 0, 0, rbuf_new, node_sets_new, edits->offset_sequence_two, edits->length, writer);

      last_diff_old = edits->offset_sequence_one + 1;
      last_diff_new = edits->offset_sequence_two + edits->length;

      break;

    case DELETE:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old, edits->offset_sequence_one, edits->length, rbuf_new, node_sets_new, 0, 0, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edits->offset_sequence_two + 1;

      break;
    }

  }

  // output diff tag start
  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;


  for(unsigned int j = last_diff_old, k = last_diff_new; j < node_sets_old->size(); ++j, ++k)
    markup_whitespace(rbuf_old, node_sets_old->at(j), rbuf_new, node_sets_new->at(k), writer);

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
  for(leftptr = 0; leftptr < node_set_old->size() && leftptr < node_set_new->size() && node_compare(nodes_old.at(node_set_old->at(leftptr)), nodes_new.at(node_set_new->at(leftptr))) == 0; ++leftptr);

  unsigned int rightptr;
  for(rightptr = 1; rightptr <= node_set_old->size() && rightptr <= node_set_new->size()
        && node_compare(nodes_old.at(node_set_old->at(node_set_old->size() - rightptr)), 
                        nodes_new.at(node_set_new->at(node_set_new->size() - rightptr))) == 0; ++rightptr);

  int old_diff = ((int)node_set_old->size() - rightptr) - leftptr;
  int new_diff = ((int)node_set_new->size() - rightptr) - leftptr;

  return ((old_diff > new_diff) ? old_diff : new_diff) + 1;
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

void compare_many2many(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , struct edit * edit_script, xmlTextWriterPtr writer) {

  struct edit * edits = edit_script;
  struct edit * edit_next = edit_script->next;

  struct offset_pair * matches;

  match_differences(node_sets_old, node_sets_new, edit_script, &matches);

  int last_old = 0;
  int last_new = 0;

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

    } else if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))
                           , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->at(0))) == 0
              && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->at(0))->type != XML_READER_TYPE_TEXT) {

      output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset
                       , rbuf_new, node_sets_new, edit_next->offset_sequence_two + matches->new_offset, writer);

      if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
        rbuf_old->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

    } else {

      output_change(rbuf_old, node_sets_old, edits->offset_sequence_one + matches->old_offset, 1,
                    rbuf_new, node_sets_new, edit_next->offset_sequence_two + matches->new_offset, 1, writer);

    }

    last_old = matches->old_offset + 1;
    last_new = matches->new_offset + 1;

  }

  // output diffs until match
  output_change(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old, (edits->length - last_old),
                rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new, (edit_next->length - last_new), writer);


}

bool output_peek(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer) {

  //fprintf(stderr, "HERE PEAK\n");

  return true;

}

void output_recursive(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old
                      , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new
                      , xmlTextWriterPtr writer) {

  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;

  output_handler(rbuf_old, rbuf_new, nodes_old.at(node_sets_old->at(start_old)->at(0)), COMMON, writer);


  // compare subset of nodes

  if(strcmp((const char *)nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    std::vector<std::vector<int> *> * next_node_set_old
      = create_comment_paragraph_set(&nodes_old, node_sets_old->at(start_old)->at(1) + 1
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 2));

  std::vector<std::vector<int> *> * next_node_set_new
    = create_comment_paragraph_set(&nodes_new, node_sets_new->at(start_new)->at(1)
                      , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 2));

    output_comment_paragraph(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

  }
  else {

    // collect subset of nodes
    std::vector<std::vector<int> *> * next_node_set_old
      = create_node_set(&nodes_old, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 2));

    std::vector<std::vector<int> *> * next_node_set_new
      = create_node_set(&nodes_new, node_sets_new->at(start_new)->at(1)
      , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 2));

    output_diffs(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

  }

  output_handler(rbuf_old, rbuf_new,
                 nodes_old.at(node_sets_old->at(start_old)->
                             at(node_sets_old->at(start_old)->size() - 1))
                 , COMMON, writer);

  if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

  output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

}


void markup_whitespace(struct reader_buffer * rbuf_old, std::vector<int> * node_set_old, struct reader_buffer * rbuf_new, std::vector<int> * node_set_new, xmlTextWriterPtr writer) {

  for(unsigned int i = 0, j = 0; i < node_set_old->size() && j < node_set_new->size(); ++i, ++j) {

    if(node_compare(nodes_old.at(node_set_old->at(i)), nodes_new.at(node_set_new->at(j))) == 0)

      output_handler(rbuf_old, rbuf_new, nodes_old.at(node_set_old->at(i)), COMMON, writer);

    else if(is_white_space(nodes_old.at(node_set_old->at(i))) && is_white_space(nodes_new.at(node_set_new->at(j)))) {

      xmlChar * content_old = nodes_old.at(node_set_old->at(i))->content;
      xmlChar * content_new = nodes_new.at(node_set_new->at(j))->content;

      int size_old = strlen((const char *)nodes_old.at(node_set_old->at(i))->content);
      int size_new = strlen((const char *)nodes_new.at(node_set_new->at(j))->content);

      int start_old = 0;
      int start_new = 0;

      for(; start_old < size_old && start_new < size_new && content_old[start_old] == content_new[start_new]; ++start_old, ++start_new);

      xmlTextWriterWriteRawLen(writer, content_old, start_old);

      if(start_old < size_old) {


        if(rbuf_old->open_diff->back()->operation != DELETE)
          output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

        // output diff tag
        //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old type=\"whitespace\">"));

        xmlTextWriterWriteRawLen(writer, content_old + start_old, size_old - start_old);

        // output diff tag
        output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

      }

      if(start_new < size_new) {

        if(rbuf_old->open_diff->back()->operation != INSERT)
          output_handler(rbuf_new, rbuf_new, diff_new_start, INSERT, writer);
        // output diff tag
        //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new type=\"whitespace\">"));

        xmlTextWriterWriteRawLen(writer, content_new + start_new, size_new - start_new);

        // output diff tag
        output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
        //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

      }

      // whitespace change
    } else if(is_white_space(nodes_old.at(node_set_old->at(i)))) {

      if(rbuf_old->open_diff->back()->operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);
      // whitespace delete
      // output diff tag
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old type=\"whitespace\">"));

      output_handler(rbuf_old, rbuf_new, nodes_old.at(node_set_old->at(i)), DELETE, writer);

      // output diff tag
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

      --j;

    } else if(is_white_space(nodes_new.at(node_set_new->at(j)))) {

      if(rbuf_old->open_diff->back()->operation != INSERT)
        output_handler(rbuf_new, rbuf_new, diff_new_start, INSERT, writer);
      //whitespace insert
      // output diff tag
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new type=\"whitespace\">"));

      output_handler(rbuf_old, rbuf_new, nodes_new.at(node_set_new->at(j)), INSERT, writer);

      // output diff tag
      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

      --i;

    } else if(is_text(nodes_old.at(node_set_old->at(i))) && is_text(nodes_new.at(node_set_new->at(j)))) {

      // collect all adjacent text nodes character arrays and input difference
      std::string text_old = "";
      for(; i < node_set_old->size() && is_text(nodes_old.at(node_set_old->at(i))); ++i)
        text_old += (const char *)nodes_old.at(node_set_old->at(i))->content;

      std::string text_new = "";
      for(; j < node_set_new->size() && is_text(nodes_new.at(node_set_new->at(j))); ++j)
        text_new += (const char *)nodes_new.at(node_set_new->at(j))->content;

      --i;
      --j;

      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, text_old.c_str());
      //fprintf(stderr, "HERE: %s %s %d %s\n", __FILE__, __FUNCTION__, __LINE__, text_new.c_str());

      for(unsigned int opos = 0, npos = 0; opos < text_old.size() && npos < text_new.size(); ++opos, ++npos) {

        if(text_old[opos] == text_new[npos]) {

          //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
          output_char(text_old[opos], writer);
          continue;
        }

        if(isspace(text_old[opos]) || isspace(text_new[npos])) {

          if(isspace(text_old[opos])) {

            if(rbuf_old->open_diff->back()->operation != DELETE)
              output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old type=\"whitespace\">"));

            for(; opos < text_old.size() && isspace(text_old[opos]); ++opos) {

              //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_old[opos]);
              output_char(text_old[opos], writer);
            }

            // output diff tag
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
            output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

          }

          if(isspace(text_new[npos])) {

            if(rbuf_old->open_diff->back()->operation != INSERT)
              output_handler(rbuf_new, rbuf_new, diff_new_start, INSERT, writer);
            xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new type=\"whitespace\">"));

            for(; npos < text_new.size() && isspace(text_new[npos]); ++npos) {

              //fprintf(stderr, "HERE: %s %s %d '%c'\n", __FILE__, __FUNCTION__, __LINE__, text_new[npos]);
              output_char(text_new[npos], writer);
            }

            output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
            // output diff tag
            //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

          }

          --opos;
          --npos;

        }

      }

    } else {

      fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    }

  }

}

/*
// not generic enough may need to pass nodes_old or nodes_new
bool is_block_type(std::vector<std::vector<int> *> * node_sets, int start, int length) {

  if(length != 1)
    return false;


  if((xmlReaderTypes)node_sets->at(start)->at(0)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)node_sets->at(start)->at(0)->name, "block") == 0)
    return true;

  return false;

}

bool is_statement_type(std::vector<std::vector<int> *> * node_sets, int start, int length) {

  if(length != 1)
    return false;

  if((xmlReaderTypes)node_sets->at(start)->at(0)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if(strcmp((const char *)node_sets->at(start)->at(0)->name, "expr_stmt") == 0)
    return true;

  return false;

}

bool is_nestable(std::vector<std::vector<int> *> * node_sets_old
                 , int start_old, int length_old
                 , std::vector<std::vector<int> *> * node_sets_new
                 , int start_new, int length_new) {

  return (is_block_type(node_sets_old, start_old, length_old) && is_statement_type(node_sets_new, start_new, length_new))
    || (is_block_type(node_sets_new, start_new, length_new) && is_statement_type(node_sets_old, start_old, length_old));
}
*/

void output_change(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                   , int start_old, int length_old
                   , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                   , int start_new, int length_new
                   , xmlTextWriterPtr writer) {

  int begin_old = start_old;
  int begin_new = start_new;
  int olength = length_old;
  int nlength = length_new;

  if(olength > 0 && nlength > 0) {

    if(is_white_space(nodes_old.at(node_sets_old->at(begin_old)->at(0))) && is_white_space(nodes_new.at(node_sets_new->at(begin_new)->at(0)))) {

      xmlChar * content_old = nodes_old.at(node_sets_old->at(begin_old)->at(0))->content;
      xmlChar * content_new = nodes_new.at(node_sets_new->at(begin_new)->at(0))->content;

      int size_old = strlen((const char *)nodes_old.at(node_sets_old->at(begin_old)->at(0))->content);
      int size_new = strlen((const char *)nodes_new.at(node_sets_new->at(begin_new)->at(0))->content);

      int offset_old = 0;
      int offset_new = 0;

      for(; offset_old < size_old && offset_new < size_new && content_old[offset_old] == content_new[offset_new]; ++offset_old, ++offset_new);

      xmlTextWriterWriteRawLen(writer, content_old, offset_old);

      if(offset_old < size_old) {

        // shrink
        nodes_old.at(node_sets_old->at(begin_old)->at(0))->content = content_old + offset_old;
        //node_sets_old->at(begin_old)->at(0)->content = (xmlChar *)strndup((const char *)(content_old + offset_old), size_old - offset_old);

      } else {

        nodes_old.at(node_sets_old->at(begin_old)->at(0))->content = (xmlChar *)"";
      }

      if(offset_new < size_new) {

        nodes_new.at(node_sets_new->at(begin_new)->at(0))->content = content_new + offset_new;

      } else {

        nodes_new.at(node_sets_new->at(begin_new)->at(0))->content = (xmlChar *)"";
      }


    }

  }

  /*
  if(0 && is_nestable(node_sets_old, begin_old, olength, node_sets_new, begin_new, nlength)) {

    if(is_block_type(node_sets_old, start_old, length_old)) {

      // output diff tag begin
      if(rbuf_old->open_diff->back()->operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

      rbuf_old->open_diff->back()->open_tags->front()->marked = false;

      output_handler(rbuf_old, rbuf_new, node_sets_old->at(begin_old)->at(0), DELETE, writer);
      output_handler(rbuf_old, rbuf_new, node_sets_old->at(begin_old)->at(1), DELETE, writer);

      // collect subset of nodes
      std::vector<std::vector<int> *> * next_node_set_old
        = create_node_set(node_sets_old->at(begin_old), 2
                          , node_sets_old->at(begin_old)->size() - 2);

      std::vector<std::vector<int> *> * next_node_set_new
        = create_node_set(node_sets_new->at(begin_new), 0
                          , node_sets_new->at(begin_new)->size());

      output_diffs(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

      output_handler(rbuf_old, rbuf_new, node_sets_old->at(begin_old)->
                     at(node_sets_old->at(begin_old)->size() - 1), DELETE, writer);

      output_handler(rbuf_old, rbuf_new, node_sets_old->at(begin_old)->
                     at(node_sets_old->at(begin_old)->size() - 2), DELETE, writer);

      // output diff tag begin
      if(rbuf_old->open_diff->back()->operation == DELETE)
        rbuf_old->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

    } else {

      // output diff tag
      if(rbuf_new->open_diff->back()->operation != INSERT)
        output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

      rbuf_new->open_diff->back()->open_tags->front()->marked = false;


      // output diff tag begin

      output_handler(rbuf_old, rbuf_new, node_sets_new->at(begin_new)->at(0), INSERT, writer);
      output_handler(rbuf_old, rbuf_new, node_sets_new->at(begin_new)->at(1), INSERT, writer);

      // collect subset of nodes
      std::vector<std::vector<int> *> * next_node_set_old
        = create_node_set(node_sets_old->at(begin_old), 0
                          , node_sets_old->at(begin_old)->size());

      std::vector<std::vector<int> *> * next_node_set_new
        = create_node_set(node_sets_new->at(begin_new), 2
                          , node_sets_new->at(begin_new)->size() - 2);

      output_diffs(rbuf_old, node_sets_old, rbuf_new, next_node_set_new, writer);

      output_handler(rbuf_old, rbuf_new, node_sets_new->at(begin_new)->
                     at(node_sets_new->at(begin_new)->size() - 1), INSERT, writer);

      output_handler(rbuf_old, rbuf_new, node_sets_new->at(begin_new)->
                     at(node_sets_new->at(begin_new)->size() - 2), INSERT, writer);

      // output diff tag begin
      if(rbuf_new->open_diff->back()->operation == INSERT)
        rbuf_new->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);

    }
      fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
  }

  // match beginning whitespace
  else {
  */
    if(olength > 0) {

      // output diff tag begin
      if(rbuf_old->open_diff->back()->operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

      rbuf_old->open_diff->back()->open_tags->front()->marked = false;

      for(int j = 0; j < olength; ++j)
        for(unsigned int i = 0; i < node_sets_old->at(begin_old + j)->size(); ++i)
          output_handler(rbuf_old, rbuf_new, nodes_old.at(node_sets_old->at(begin_old + j)->at(i)), DELETE, writer);

      // output diff tag begin
      if(rbuf_old->open_diff->back()->operation == DELETE)
        rbuf_old->open_diff->back()->open_tags->front()->marked = true;

      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

    }

    if(nlength > 0) {

      // output diff tag
      if(rbuf_new->open_diff->back()->operation != INSERT)
        output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

      rbuf_new->open_diff->back()->open_tags->front()->marked = false;

      for(int j = 0; j < nlength; ++j)
        for(unsigned int i = 0; i < node_sets_new->at(begin_new + j)->size(); ++i)
          output_handler(rbuf_old, rbuf_new, nodes_new.at(node_sets_new->at(begin_new + j)->at(i)), INSERT, writer);

      // output diff tag begin
      if(rbuf_new->open_diff->back()->operation == INSERT)
        rbuf_new->open_diff->back()->open_tags->front()->marked = true;
      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);

    }

    //  }

}

bool is_change(struct edit * edit_script) {

  return edit_script->operation == DELETE && edit_script->next != NULL && edit_script->next->operation == INSERT
    && (edit_script->offset_sequence_one + edit_script->length - 1) == edit_script->next->offset_sequence_one;

}

/*

bool only_white_space_between_change(struct edit * change_one, struct edit * change_two, std::vector<std::vector<int> *> * node_sets_old) {

  for(int set = change_one->offset_sequence_one + change_one->length; set < change_two->offset_sequence_one; ++set)
    if(!is_white_space_set(node_sets_old->at(set)))
      return false;

  return true;
}

void group_changes(struct edit * edit_script, std::vector<std::vector<int> *> * node_sets_old
                   , std::vector<std::vector<int> *> * node_sets_new) {

  struct edit * edits = edit_script;

  for(; edits; edits = edits->next) {

    if(is_change(edits)) {

      struct edit * next_pair = edits->next->next;
      while(next_pair && next_pair->next && is_change(next_pair) && only_white_space_between_change(edits, next_pair, node_sets_old)) {

        edits->length = next_pair->offset_sequence_one + next_pair->length - edits->offset_sequence_one;
        edits->next->length = next_pair->next->offset_sequence_two + next_pair->next->length - edits->next->offset_sequence_two;
        next_pair = edits->next->next;
      }

    }

  }

}
*/

void output_char(char character, xmlTextWriterPtr writer) {

  if(character == '&')
    xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&amp;", 5);

  else if (character == '<')
    xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&lt;", 4);

  else if (character == '>')

    xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&gt;", 4);

  else
    xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) &character, 1);

}
