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

xmlNs diff = { 0, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", 0 };

// global structures to hold read in nodes
std::vector<xmlNode *> nodes_old;
std::vector<xmlNode *> nodes_new;

bool is_change(struct edit * edit_script) {

  return edit_script->operation == DELETE && edit_script->next != NULL && edit_script->next->operation == INSERT
    && (edit_script->offset_sequence_one + edit_script->length - 1) == edit_script->next->offset_sequence_one;

}

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
const void * node_set_index(int idx, const void *s) {
  std::vector<std::vector<int> *> & node_sets = *(std::vector<std::vector<int> *> *)s;
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
int node_compare(xmlNode * node1, xmlNode * node2) {

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
  std::vector<int> * node_set1 = (std::vector<int> *)e1;
  std::vector<int> * node_set2 = (std::vector<int> *)e2;

  if(node_set1->size() != node_set2->size())
    return 1;
  else
    for(unsigned int i = 0; i < node_set1->size(); ++i)
      if(node_compare(nodes_old.at(node_set1->at(i)), nodes_new.at(node_set2->at(i))))
        return 1;

  return 0;
}

bool is_white_space(xmlNodePtr node) {

  // TODO:  Comment on how checking the first character is enough to determine that a
  // node is all whitespace
  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && isspace((char)node->content[0]);

}

// TODO:  Rename to is_white_space
bool is_white_space_set(std::vector<int> * node_set, std::vector<xmlNodePtr> * nodes) {

  for(unsigned int i = 0; i < node_set->size(); ++i)
    if((xmlReaderTypes)nodes->at(node_set->at(i))->type != XML_READER_TYPE_TEXT || !isspace((char)nodes->at(node_set->at(i))->content[0]))
      return false;

  return true;

}

bool contains_new_line(xmlNodePtr node) {

  unsigned int length = strlen((const char *)node->content);

  for(unsigned int i = 0; i < length; ++i)
    if(node->content[i] == '\n')
      return true;

  return false;

}

bool is_text(xmlNodePtr node) {

  return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT;
}

// diff node comparison function
int node_set_syntax_compare(const void * e1, const void * e2) {
  std::vector<int> * node_set1 = (std::vector<int> *)e1;
  std::vector<int> * node_set2 = (std::vector<int> *)e2;

  for(unsigned int i = 0, j = 0; i < node_set1->size() && j < node_set2->size(); ++i, ++j) {

    // string consecutive non whitespace text nodes
    // TODO:  Why create the string?  Just compare directly as you go through
    if(is_text(nodes_old.at(node_set1->at(i))) && is_text(nodes_new.at(node_set2->at(j)))) {

      std::string text1 = "";
      for(; i < node_set1->size() && is_text(nodes_old.at(node_set1->at(i))); ++i)
        if(!is_white_space(nodes_old.at(node_set1->at(i))))
          text1 += (const char *)nodes_old.at(node_set1->at(i))->content;

      std::string text2 = "";
      for(; j < node_set2->size() && is_text(nodes_new.at(node_set2->at(j))); ++j)
        if(!is_white_space(nodes_new.at(node_set2->at(j))))
          text2 += (const char *)nodes_new.at(node_set2->at(j))->content;

      if(text1 != text2)
        return 1;

      --i;
      --j;

      // TODO:  Remove
      continue;

    }

    if(node_compare(nodes_old.at(node_set1->at(i)), nodes_new.at(node_set2->at(j))))
      return 1;
  }

  return 0;
}

// converts source code to srcML
xmlBuffer * translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

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
  int last_output;
  // TODO:  There is no reason that this the vector is a pointer.
  std::vector<struct open_diff *> * open_diff;
  std::vector<struct open_diff *> * output_diff;

};

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlTextReaderPtr reader_old, xmlTextReaderPtr reader_new);

// create sets of nodes
std::vector<std::vector<int> *> * create_node_set(std::vector<xmlNodePtr> * nodes, int start, int end);

// collect the differnces
void collect_difference(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader);

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

void output_change(struct reader_buffer * rbuf_old, int end_old, struct reader_buffer * rbuf_new, int end_new
                   , xmlTextWriterPtr writer);

void compare_many2many(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                       , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                       , struct edit * edit_script, xmlTextWriterPtr writer);

void output_handler(struct reader_buffer * rbuf_old, struct reader_buffer * rbuf_new, xmlNodePtr node, int operation, xmlTextWriterPtr writer);

void update_diff_stack(std::vector<struct open_diff *> * open_diffs, xmlNodePtr node, int operation);

void markup_whitespace(struct reader_buffer * rbuf_old, int end_old, struct reader_buffer * rbuf_new, int end_new, xmlTextWriterPtr writer);

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
    // TODO:  Remove old reading code
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
    xmlTextReaderRead(reader_old);

    struct reader_buffer rbuf_new = { 0 };
    rbuf_new.stream_source = INSERT;
    rbuf_new.open_diff = new std::vector<struct open_diff *>;

    new_diff = new struct open_diff;
    new_diff->operation = COMMON;
    new_diff->open_tags = new std::vector<struct tag *>;
    rbuf_new.open_diff->push_back(new_diff);

    rbuf_new.output_diff = &output_diff;
    xmlTextReaderRead(reader_new);

    // create srcdiff unit
    xmlNodePtr unit = create_srcdiff_unit(reader_old, reader_new);

    // output srcdiff unit
    output_handler(&rbuf_old, &rbuf_new, unit, COMMON, writer);

    int is_old = xmlTextReaderRead(reader_old);
    int is_new = xmlTextReaderRead(reader_new);

    if(is_old)
      collect_difference(&nodes_old, reader_old);

    xmlBufferFree(output_file_one);

    if(is_new)
      collect_difference(&nodes_new, reader_new);

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
void collect_difference(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader) {

  // save beginning of characters
  unsigned char * characters_start = 0;
  unsigned char * characters = 0;

  int not_done = 1;
  while(not_done)

    // look if in text node
    if(xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE || xmlTextReaderNodeType(reader) == XML_READER_TYPE_TEXT) {

      // allocate character buffer if empty and set start of characters
      if(!characters) {

        characters_start = (unsigned char *)xmlTextReaderConstValue(reader);
        characters = characters_start;
      }

      // cycle through characters
      for (; (*characters) != 0; ++characters) {

        // separte non whitespace
        // TODO:  Use isspace here
        if((*characters) != ' ' && (*characters) != '\t' && (*characters) != '\r' && (*characters) != '\n') {

          // output previous whitespace
          if(characters != characters_start) {
            xmlNode * text = new xmlNode;
            text->type = (xmlElementType)XML_READER_TYPE_TEXT;
            text->name = (const xmlChar *)"text";

            const char * content = strndup((const char *)characters_start, characters  - characters_start);

            text->content = (xmlChar *)content;
            nodes->push_back(text);

            characters_start = characters;

          }

          // TODO:  Use isspace here
          while((*characters) != 0 && (*characters) != ' ' && (*characters) != '\t' && (*characters) != '\r' && (*characters) != '\n')
            ++characters;

          // output other
          xmlNode * text = new xmlNode;
          text->type = (xmlElementType)XML_READER_TYPE_TEXT;
          text->name = (const xmlChar *)"text";

          const char * content = strndup((const char *)characters_start, characters  - characters_start);
          text->content = (xmlChar *)content;
          nodes->push_back(text);
          characters_start = characters;

          if(!*characters)
            break;

        }

        // increase new line count and check if end of diff
        if((*characters) == '\n') {

          xmlNode * text = new xmlNode;
          text->type = (xmlElementType)XML_READER_TYPE_TEXT;
          text->name = (const xmlChar *)"text";

          const char * content = strndup((const char *)characters_start, (characters + 1) - characters_start);
          text->content = (xmlChar *)content;
          nodes->push_back(text);
          characters_start = characters + 1;

        }

      }

      // end and save text node if finished and get next node
      if(!(*characters)) {

        // create new node and buffer it
        if(characters != characters_start) {

          xmlNode * text = new xmlNode;
          text->type = (xmlElementType)XML_READER_TYPE_TEXT;
          text->name = (const xmlChar *)"text";

          const char * content = strdup((const char *)characters_start);
          text->content = (xmlChar *)content;
          nodes->push_back(text);

        }

        characters = NULL;

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

void collect_entire_tag(std::vector<xmlNodePtr> * nodes, std::vector<int> * node_set, int * start) {

  //const char * open_node = (const char *)nodes->at(*start)->name;

  node_set->push_back(*start);

  if(nodes->at(*start)->extra & 0x1)
    return;

  ++(*start);

  std::vector<bool> is_open;

  is_open.push_back(false);

  for(; !is_open.empty(); ++(*start)) {

    if(is_white_space(nodes->at(*start)))
    continue;

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

    if(is_white_space(nodes->at(i))) {

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->content);
      continue;
      node_set->push_back(i);

    } else if((xmlReaderTypes)nodes->at(i)->type == XML_READER_TYPE_TEXT) {
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes->at(i)->content);
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

void output_common(struct reader_buffer * rbuf_old, int end_old
                   , struct reader_buffer * rbuf_new, int end_new
                   , xmlTextWriterPtr writer) {

  int oend = end_old;
  int nend = end_new;

  for(; oend < nodes_old.size() && is_white_space(nodes_old.at(oend)) && contains_new_line(nodes_old.at(oend)); ++oend);
  for(; nend < nodes_new.size() && is_white_space(nodes_new.at(nend)) && contains_new_line(nodes_new.at(nend)); ++nend); 

  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;

  // add preceeding unchanged
  markup_whitespace(rbuf_old, oend, rbuf_new, nend, writer);

  if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

  output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

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
  int diff_end_old = rbuf_old->last_output;
  int diff_end_new = rbuf_new->last_output;

  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {

    diff_end_old = rbuf_old->last_output;
    diff_end_new = rbuf_new->last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

      output_common(rbuf_old, diff_end_old

                    , rbuf_new, diff_end_new

                    , writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      //      fprintf(stderr, "HERE\n");

      if(edits->length == edit_next->length && edits->length == 1
                && (node_sets_old->at(edits->offset_sequence_one)->size() > 1
                    || node_sets_old->at(edits->offset_sequence_one)->size() > 1)) {

        if(node_compare(nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))
                        , nodes_new.at(node_sets_new->at(edit_next->offset_sequence_two)->at(0))) == 0
           && (xmlReaderTypes)nodes_old.at(node_sets_old->at(edits->offset_sequence_one)->at(0))->type != XML_READER_TYPE_TEXT) {

          output_recursive(rbuf_old, node_sets_old, edits->offset_sequence_one
                           , rbuf_new, node_sets_new, edit_next->offset_sequence_two, writer);

        } else {

          output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one)->back() + 1
                        , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two)->back() + 1, writer);

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
      output_change(rbuf_old, 0
                    , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, writer);


      last_diff_old = edits->offset_sequence_one + 1;
      last_diff_new = edits->offset_sequence_two + edits->length;

      break;

    case DELETE:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                    , rbuf_new, 0, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edits->offset_sequence_two + 1;

      break;
    }

  }

  diff_end_old = rbuf_old->last_output;
  diff_end_new = rbuf_new->last_output;
  if(last_diff_old < node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }
    output_common(rbuf_old, node_sets_old->back()->back() + 1

                  , rbuf_new, node_sets_new->back()->back() + 1

                  , writer);

  free_shortest_edit_script(edit_script);

}

std::vector<std::vector<int> *> * create_comment_paragraph_set(std::vector<xmlNodePtr> * nodes, int start, int end) {

  std::vector<std::vector<int> *> * node_sets = new std::vector<std::vector<int> *>;

  for(int i = start; i < end; ++i) {

    std::vector <int> * node_set = new std::vector <int>;

    if(contains_new_line(nodes->at(i))) {

      for(; contains_new_line(nodes->at(i)); ++i);
      //node_set->push_back(i);

      continue;
      --i;

    } else {

      bool first_newline = false;
      for(; i < end; ++i) {

        if(first_newline && contains_new_line(nodes->at(i))) {

          --i;
          break;

        } else
          first_newline = false;


        if(!first_newline && contains_new_line(nodes->at(i)))
          first_newline = true;

        if(is_white_space(nodes->at(i)))
          continue;

        node_set->push_back(i);
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

      if(contains_new_line(nodes->at(i)))
        break;

      if(is_white_space(nodes->at(i)))
        continue;

      node_set->push_back(i);
    }

    node_sets->push_back(node_set);

  }

  return node_sets;

}

void output_comment_paragraph(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old, struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new, xmlTextWriterPtr writer) {

  struct edit * edit_script;
  int distance = shortest_edit_script(node_sets_old->size(), (void *)node_sets_old, node_sets_new->size(),
                                      (void *)node_sets_new, node_set_syntax_compare, node_set_index, &edit_script);

  if(distance < 0) {

    fprintf(stderr, "Error with shortest edit script");
    exit(distance);
  }

  int last_diff_old = 0;
  int last_diff_new = 0;
  int diff_end_old = rbuf_old->last_output;
  int diff_end_new = rbuf_new->last_output;

  struct edit * edits = edit_script;
  for (; edits; edits = edits->next) {



    // add preceeding unchanged
    diff_end_old = rbuf_old->last_output;
    diff_end_new = rbuf_new->last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

      output_common(rbuf_old, diff_end_old

                    , rbuf_new, diff_end_new

                    , writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> * next_node_set_old
          = create_comment_line_set(&nodes_old, node_sets_old->at(edits->offset_sequence_one)->at(0)
                                    , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        std::vector<std::vector<int> *> * next_node_set_new
          = create_comment_line_set(&nodes_new, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                                    , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        output_comment_line(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

      } else {

        output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, writer);
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
      output_change(rbuf_old, 0
                    , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, writer);

      last_diff_old = edits->offset_sequence_one + 1;
      last_diff_new = edits->offset_sequence_two + edits->length;

      break;

    case DELETE:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                    , rbuf_new, 0, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edits->offset_sequence_two + 1;

      break;
    }

  }

  diff_end_old = rbuf_old->last_output;
  diff_end_new = rbuf_new->last_output;
  if(last_diff_old < node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }
    output_common(rbuf_old, node_sets_old->back()->back() + 1

                  , rbuf_new, node_sets_new->back()->back() + 1

                  , writer);

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
  int diff_end_old = rbuf_old->last_output;
  int diff_end_new = rbuf_new->last_output;

  struct edit * edits = edit_script;

  for (; edits; edits = edits->next) {

    // add preceeding unchanged
    diff_end_old = rbuf_old->last_output;
    diff_end_new = rbuf_new->last_output;
    if(edits->operation == DELETE && last_diff_old < edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one - 1)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old) - 1)->back() + 1;

    } else if(edits->operation == INSERT && last_diff_old <= edits->offset_sequence_one) {

      diff_end_old = node_sets_old->at(edits->offset_sequence_one)->back() + 1;
      diff_end_new = node_sets_new->at(last_diff_new + (edits->offset_sequence_one - last_diff_old))->back() + 1;
    }

      output_common(rbuf_old, diff_end_old

                    , rbuf_new, diff_end_new

                    , writer);

    // detect and change
    struct edit * edit_next = edits->next;
    if(is_change(edits)) {

      if(edits->length == 1 && edit_next->length == 1) {

        // collect subset of nodes
        std::vector<std::vector<int> *> * next_node_set_old
          = create_node_set(&nodes_old, node_sets_old->at(edits->offset_sequence_one)->at(0)
                            , node_sets_old->at(edits->offset_sequence_one)->at(node_sets_old->at(edits->offset_sequence_one)->size() - 1) + 1);

        std::vector<std::vector<int> *> * next_node_set_new
          = create_node_set(&nodes_new, node_sets_new->at(edit_next->offset_sequence_two)->at(0)
                            , node_sets_new->at(edit_next->offset_sequence_two)->at(node_sets_new->at(edit_next->offset_sequence_two)->size() - 1) + 1);

        output_diffs(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

      } else
        output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                      , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + edit_next->length - 1)->back() + 1, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edit_next->offset_sequence_two + edit_next->length;
      edits = edits->next;
      continue;
    }

    // handle pure delete or insert
    switch (edits->operation) {

    case INSERT:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, 0
                    , rbuf_new, node_sets_new->at(edits->offset_sequence_two + edits->length - 1)->back() + 1, writer);

      last_diff_old = edits->offset_sequence_one + 1;
      last_diff_new = edits->offset_sequence_two + edits->length;

      break;

    case DELETE:

      //fprintf(stderr, "HERE\n");
      output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + edits->length - 1)->back() + 1
                    , rbuf_new, 0, writer);

      last_diff_old = edits->offset_sequence_one + edits->length;
      last_diff_new = edits->offset_sequence_two + 1;

      break;
    }

  }

  diff_end_old = rbuf_old->last_output;
  diff_end_new = rbuf_new->last_output;
  if(last_diff_old < node_sets_old->size()) {

    diff_end_old = node_sets_old->back()->back() + 1;
    diff_end_new = node_sets_new->back()->back() + 1;

  }
    output_common(rbuf_old, node_sets_old->back()->back() + 1

                  , rbuf_new, node_sets_new->back()->back() + 1

                  , writer);

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

  if(node_set_compare(node_set_old, node_set_new) == 0)
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

void output_unmatched(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old, int end_old
                      , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new, int end_new
                      , xmlTextWriterPtr writer) {

  int finish_old = 0;
  int finish_new = 0;

  if(start_old <= end_old && start_old >= 0 && end_old < node_sets_old->size()) {

    finish_old = node_sets_old->at(end_old)->back() + 1;
  }

  if(start_new <= end_new && start_new >= 0 && end_new < node_sets_new->size()) {

    finish_new = node_sets_new->at(end_new)->back() + 1;
  }

  output_change(rbuf_old, finish_old, rbuf_new, finish_new, writer);

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
    output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                     edits->offset_sequence_one + matches->old_offset - 1,
                     rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                     , edit_next->offset_sequence_two + matches->new_offset - 1, writer);

    // correct could only be whitespace
    if(matches->similarity == MIN) {

      if(rbuf_old->open_diff->back()->operation != COMMON)
        output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

      rbuf_old->open_diff->back()->open_tags->front()->marked = false;

      markup_whitespace(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1

                        , rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1
                        , writer);

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

      output_change(rbuf_old, node_sets_old->at(edits->offset_sequence_one + matches->old_offset)->back() + 1,
                    rbuf_new, node_sets_new->at(edit_next->offset_sequence_two + matches->new_offset)->back() + 1
                    , writer);

    }

    last_old = matches->old_offset + 1;
    last_new = matches->new_offset + 1;

  }

  // output diffs until match
  output_unmatched(rbuf_old, node_sets_old, edits->offset_sequence_one + last_old,
                   edits->offset_sequence_one + edits->length - 1,
                   rbuf_new, node_sets_new, edit_next->offset_sequence_two + last_new
                   , edit_next->offset_sequence_two + edit_next->length - 1, writer);


}

void output_recursive(struct reader_buffer * rbuf_old, std::vector<std::vector<int> *> * node_sets_old
                      , int start_old
                      , struct reader_buffer * rbuf_new, std::vector<std::vector<int> *> * node_sets_new
                      , int start_new
                      , xmlTextWriterPtr writer) {

  if(rbuf_old->open_diff->back()->operation != COMMON)
    output_handler(rbuf_old, rbuf_new, diff_common_start, COMMON, writer);

  rbuf_old->open_diff->back()->open_tags->front()->marked = false;

  markup_whitespace(rbuf_old, node_sets_old->at(start_old)->at(0), rbuf_new, node_sets_new->at(start_new)->at(0), writer);

  output_handler(rbuf_old, rbuf_new, nodes_old.at(node_sets_old->at(start_old)->at(0)), COMMON, writer);

  ++rbuf_old->last_output;
  ++rbuf_new->last_output;

  // compare subset of nodes

  if(strcmp((const char *)nodes_old.at(node_sets_old->at(start_old)->at(0))->name, "comment") == 0) {

    // collect subset of nodes
    std::vector<std::vector<int> *> * next_node_set_old
      = create_comment_paragraph_set(&nodes_old, node_sets_old->at(start_old)->at(1)
                                     , node_sets_old->at(start_old)->at(node_sets_old->at(start_old)->size() - 1));

    std::vector<std::vector<int> *> * next_node_set_new
      = create_comment_paragraph_set(&nodes_new, node_sets_new->at(start_new)->at(1)
                                     , node_sets_new->at(start_new)->at(node_sets_new->at(start_new)->size() - 1));

    output_comment_paragraph(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);

  }
  else {

    // collect subset of nodes
    std::vector<std::vector<int> *> * next_node_set_old
      = create_node_set(&nodes_old, node_sets_old->at(start_old)->at(1)
                        , node_sets_old->at(start_old)->back());

    std::vector<std::vector<int> *> * next_node_set_new
      = create_node_set(&nodes_new, node_sets_new->at(start_new)->at(1)
                        , node_sets_new->at(start_new)->back());

    output_diffs(rbuf_old, next_node_set_old, rbuf_new, next_node_set_new, writer);


  }

  markup_whitespace(rbuf_old, node_sets_old->at(start_old)->back(), rbuf_new, node_sets_new->at(start_new)->back(), writer);

  output_handler(rbuf_old, rbuf_new,
                 nodes_old.at(node_sets_old->at(start_old)->
                              at(node_sets_old->at(start_old)->size() - 1))
                 , COMMON, writer);

  ++rbuf_old->last_output;
  ++rbuf_new->last_output;

  if(rbuf_old->open_diff->back()->operation == COMMON && rbuf_old->open_diff->size() > 1)
    rbuf_old->open_diff->back()->open_tags->front()->marked = true;

  output_handler(rbuf_old, rbuf_new, diff_common_end, COMMON, writer);

}


void markup_whitespace(struct reader_buffer * rbuf_old, int end_old, struct reader_buffer * rbuf_new, int end_new, xmlTextWriterPtr writer) {

  int begin_old = rbuf_old->last_output;
  int begin_new = rbuf_new->last_output;

  int oend = end_old;
  int nend = end_new;

  unsigned int i, j;
  for(i = begin_old, j = begin_new; i < oend && j < nend; ++i, ++j) {

    if(node_compare(nodes_old.at(i), nodes_new.at(j)) == 0)

      output_handler(rbuf_old, rbuf_new, nodes_old.at(i), COMMON, writer);

    else if(is_white_space(nodes_old.at(i)) && is_white_space(nodes_new.at(j))) {

      xmlChar * content_old = nodes_old.at(i)->content;
      xmlChar * content_new = nodes_new.at(j)->content;

      int size_old = strlen((const char *)content_old);
      int size_new = strlen((const char *)content_new);

      int ostart = 0;
      int nstart = 0;

      for(; ostart < size_old && nstart < size_new && content_old[ostart] == content_new[nstart]; ++ostart, ++nstart);

      xmlTextWriterWriteRawLen(writer, content_old, ostart);

      if(ostart < size_old) {


        if(rbuf_old->open_diff->back()->operation != DELETE)
          output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

        // output diff tag
        //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old type=\"whitespace\">"));

        xmlTextWriterWriteRawLen(writer, content_old + ostart, size_old - ostart);

        // output diff tag
        output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

      }

      if(nstart < size_new) {

        if(rbuf_old->open_diff->back()->operation != INSERT)
          output_handler(rbuf_new, rbuf_new, diff_new_start, INSERT, writer);
        // output diff tag
        //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new type=\"whitespace\">"));

        xmlTextWriterWriteRawLen(writer, content_new + nstart, size_new - nstart);

        // output diff tag
        output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
        //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

      }

      // whitespace change
    } else if(is_white_space(nodes_old.at(i))) {

      if(rbuf_old->open_diff->back()->operation != DELETE)
        output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);
      // whitespace delete
      // output diff tag
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old type=\"whitespace\">"));

      output_handler(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, writer);

      // output diff tag
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
      output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

      --j;

    } else if(is_white_space(nodes_new.at(j))) {

      if(rbuf_old->open_diff->back()->operation != INSERT)
        output_handler(rbuf_new, rbuf_new, diff_new_start, INSERT, writer);
      //whitespace insert
      // output diff tag
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new type=\"whitespace\">"));

      output_handler(rbuf_old, rbuf_new, nodes_new.at(j), INSERT, writer);

      // output diff tag
      output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);
      //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));

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

      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes_old.at(i)->name);
      //fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, (const char *)nodes_new.at(i)->name);
      fprintf(stderr, "HERE: %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    }

  }

  if(i < oend) {

    if(rbuf_old->open_diff->back()->operation != DELETE)
      output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);
    // whitespace delete
    // output diff tag
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:old type=\"whitespace\">"));

    for( ; i < oend; ++i)
      output_handler(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, writer);

    // output diff tag
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:old>"));
    output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

  } else if(j < nend) {

    if(rbuf_new->open_diff->back()->operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);
    // whitespace delete
    // output diff tag
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("<diff:new type=\"whitespace\">"));

    for( ; j < nend; ++j)
      output_handler(rbuf_old, rbuf_new, nodes_new.at(j), INSERT, writer);

    // output diff tag
    //xmlTextWriterWriteRawLen(writer, LITERALPLUSSIZE("</diff:new>"));
    output_handler(rbuf_old, rbuf_new, diff_new_end, DELETE, writer);

  }

    rbuf_old->last_output = oend;
    rbuf_new->last_output = nend;

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

void output_change(struct reader_buffer * rbuf_old, int end_old
                   , struct reader_buffer * rbuf_new, int end_new
                   , xmlTextWriterPtr writer) {

  int begin_old = rbuf_old->last_output;
  int begin_new = rbuf_new->last_output;
  int oend = end_old;
  int nend = end_new;

  for(; oend < nodes_old.size() && is_white_space(nodes_old.at(oend)) && contains_new_line(nodes_old.at(oend)); ++oend);
  for(; nend < nodes_new.size() && is_white_space(nodes_new.at(nend)) && contains_new_line(nodes_new.at(nend)); ++nend); 

  if(oend > begin_old && nend > begin_new) {

    if(is_white_space(nodes_old.at(begin_old)) && is_white_space(nodes_new.at(begin_new))) {

      xmlChar * content_old = nodes_old.at(begin_old)->content;
      xmlChar * content_new = nodes_new.at(begin_new)->content;

      int size_old = strlen((const char *)content_old);
      int size_new = strlen((const char *)content_new);

      int offset_old = 0;
      int offset_new = 0;

      for(; offset_old < size_old && offset_new < size_new && content_old[offset_old] == content_new[offset_new]; ++offset_old, ++offset_new);

      xmlTextWriterWriteRawLen(writer, content_old, offset_old);

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

  /*
    if(0 && is_nestable(node_sets_old, begin_old, oend, node_sets_new, begin_new, nend)) {

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

  if(oend > begin_old) {

    // output diff tag begin
    if(rbuf_old->open_diff->back()->operation != DELETE)
      output_handler(rbuf_old, rbuf_new, diff_old_start, DELETE, writer);

    rbuf_old->open_diff->back()->open_tags->front()->marked = false;

    for(unsigned int i = begin_old; i < oend; ++i)
      output_handler(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, writer);

    // output diff tag begin
    if(rbuf_old->open_diff->back()->operation == DELETE)
      rbuf_old->open_diff->back()->open_tags->front()->marked = true;

    output_handler(rbuf_old, rbuf_new, diff_old_end, DELETE, writer);

    rbuf_old->last_output = oend;

  }

  if(nend > begin_new) {

    // output diff tag
    if(rbuf_new->open_diff->back()->operation != INSERT)
      output_handler(rbuf_old, rbuf_new, diff_new_start, INSERT, writer);

    rbuf_new->open_diff->back()->open_tags->front()->marked = false;

    for(unsigned int i = begin_new; i < nend; ++i)
      output_handler(rbuf_old, rbuf_new, nodes_new.at(i), INSERT, writer);

    // output diff tag begin
    if(rbuf_new->open_diff->back()->operation == INSERT)
      rbuf_new->open_diff->back()->open_tags->front()->marked = true;
    output_handler(rbuf_old, rbuf_new, diff_new_end, INSERT, writer);

    rbuf_new->last_output = nend;

  }

  //  }

}

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
