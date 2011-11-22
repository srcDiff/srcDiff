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
#include "srcDiffTypes.hpp"
#include "srcDiffWhiteSpace.hpp"
#include "srcDiffCommon.hpp"
#include "srcDiffChange.hpp"
#include "srcDiffOutput.hpp"
#include "srcDiffDiff.hpp"

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#ifdef __MINGW32__
#include "mingw32.hpp"
#endif

#include "xmlrw.h"

const char* XML_DECLARATION_STANDALONE = "yes";
const char* XML_VERSION = "1.0";

const char* DIFF_PREFIX = "diff:";
const char* DIFF_OLD = "diff:delete";
const char* DIFF_NEW = "diff:insert";
const char* DIFF_COMMON = "diff:common";

// TODO:  Get the output encoding from the input
const char* output_encoding = "UTF-8";

xmlNode diff_common_start;
xmlNode diff_common_end;
xmlNode diff_old_start;
xmlNode diff_old_end;
xmlNode diff_new_start;
xmlNode diff_new_end;

xmlNs diff = { 0, XML_LOCAL_NAMESPACE, (const xmlChar *)"http://www.sdml.info/srcDiff", (const xmlChar *)"diff", 0 };

/*
  Global structures to store of the collected xml nodes.
  Base reference structure for all node comparison and output
*/
std::vector<xmlNode *> nodes_old;
std::vector<xmlNode *> nodes_new;

// tags that can have something nested in them
const char * block_types[] = { "block", "if", "while", "for", "function", 0 };

// tags that can be nested in something else (incomplete)
const char * nest_types[] = { "block", "expr_stmt", "decl_stmt", 0 };

// converts source code to srcML
xmlBuffer * translate_to_srcML(const char * source_file, const char * srcml_file, const char * dir);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlNodePtr unit_old, xmlNodePtr unit_new);


// collect the nodes
void collect_nodes(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader);

bool is_nestable(std::vector<int> * structure_one, std::vector<xmlNodePtr> & nodes_one
                 , std::vector<int> * structure_two, std::vector<xmlNodePtr> & nodes_two);

bool has_interal_block(std::vector<int> * structure, std::vector<xmlNodePtr> & nodes);

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate);

int main(int argc, char * argv[]) {

  // test for correct input
  if(argc < 3) {

    fprintf(stderr, "Usage: srcdiff oldFile newFile dir\n");
    return 1;
  }

  bool is_srcML = strcmp(argv[1], "--srcml") == 0;

  const char * srcdiff_file = "-";

  // TODO: Error handling? Is the return NULL if bad?

  /*

    Input for file one

  */

  // create the reader for the old file
  xmlTextReaderPtr reader_old = NULL;
  xmlBuffer * output_file = NULL;
  if(!is_srcML) {

    // translate file one
    output_file = translate_to_srcML(argv[1], 0, argv[3]);
    reader_old = xmlReaderForMemory((const char*) xmlBufferContent(output_file), output_file->use, 0, 0, 0);

  } else {

    reader_old = xmlNewTextReaderFilename(argv[2]);

  }

  if (reader_old == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", argv[1]);

    exit(1);
  }

  // read to unit
  xmlTextReaderRead(reader_old);

  xmlNodePtr unit_old = getRealCurrentNode(reader_old);

  // Read past unit tag open
  int is_old = xmlTextReaderRead(reader_old);

  // collect if non empty files
  xmlNodePtr unit_end = NULL;
  if(is_old) {

    collect_nodes(&nodes_old, reader_old);
    unit_end = getRealCurrentNode(reader_old);

  }

  if(!is_srcML)
    xmlBufferEmpty(output_file);

  xmlFreeTextReader(reader_old);

  // group nodes
  std::vector<std::vector<int> *> node_set_old = create_node_set(nodes_old, 0, nodes_old.size());


  /*

    Input for file two

  */

  xmlTextReaderPtr reader_new = NULL;
  if(!is_srcML) {

    // translate file two
    output_file = translate_to_srcML(argv[2], 0, argv[3]);

    // create the reader for the new file
    reader_new = xmlReaderForMemory((const char*) xmlBufferContent(output_file), output_file->use, 0, 0, 0);

  } else {

    reader_new = xmlNewTextReaderFilename(argv[3]);

  }

  if (reader_new == NULL) {

    fprintf(stderr, "Unable to open file '%s' as XML", argv[2]);

    exit(1);
  }

  // read to unit
  xmlTextReaderRead(reader_new);


  xmlNodePtr unit_new = getRealCurrentNode(reader_new);

  int is_new = xmlTextReaderRead(reader_new);

  // collect if non empty files
  if(is_new) {

    collect_nodes(&nodes_new, reader_new);

    unit_end = getRealCurrentNode(reader_new);

  }
 
  // free the buffer
  if(!is_srcML)
    xmlBufferFree(output_file);

  xmlFreeTextReader(reader_new);
 
  std::vector<std::vector<int> *> node_set_new = create_node_set(nodes_new, 0, nodes_new.size());


  /*

    Setup output file

  */

  // create the writer
  xmlTextWriterPtr writer = NULL;
  writer = xmlNewTextWriterFilename(srcdiff_file, 0);
  if (writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", srcdiff_file);

    exit(1);
  }

  /*

    Setup readers and writer.

   */

  // delete reader state
  reader_state rbuf_old = { 0 };
  rbuf_old.stream_source = DELETE;

  diff_set * new_diff = new diff_set;
  new_diff->operation = COMMON;
  rbuf_old.open_diff.push_back(new_diff);

  // insert reader state
  reader_state rbuf_new = { 0 };
  rbuf_new.stream_source = INSERT;

  new_diff = new diff_set;
  new_diff->operation = COMMON;
  rbuf_new.open_diff.push_back(new_diff);

  // writer state
  writer_state wstate = { 0 };
  wstate.writer = writer;

  new_diff = new diff_set;
  new_diff->operation = COMMON;
  wstate.output_diff.push_back(new_diff);

  // diff tags
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

  /*

    Output srcDiff

  */

  // issue the xml declaration
  xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);

  // create srcdiff unit
  xmlNodePtr unit = create_srcdiff_unit(unit_old, unit_new);

  // output srcdiff unit
  output_node(rbuf_old, rbuf_new, unit, COMMON, wstate);

  // run on file level
  if(is_old || is_new)
    output_diffs(rbuf_old, &node_set_old, rbuf_new, &node_set_new, wstate);

  // output remaining whitespace
  output_white_space_all(rbuf_old, rbuf_new, wstate);

  // output srcdiff unit ending tag
  if(is_old && is_new)
    output_node(rbuf_old, rbuf_new, unit_end, COMMON, wstate);

  // cleanup writer
  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

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
  translator.translate(NULL, dir, source_file, NULL, language);

  // close the input file
  translator.close();

  return output_buffer;
}

// collect the differnces
void collect_nodes(std::vector<xmlNode *> * nodes, xmlTextReaderPtr reader) {

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

          //while((*characters) != 0 && *characters != '\n' && isspace(*characters))
            ++characters;

	    // kind of want a look up table for this
	    //const char * content = strndup((const char *)characters_start, characters  - characters_start);
	    const char * content = strndup((const char *)characters_start, 1);
	    text->content = (xmlChar *)content;

        }

        // separate non whitespace
        else {

	  // collect all 
          while((*characters) != 0 && !isspace(*characters))
            ++characters;

	  // break up ( and )
          if((characters_start + 1) && (*characters_start) == '(' && (*(characters_start + 1)) == ')') {

            xmlNode * atext = new xmlNode;
            atext->type = (xmlElementType)XML_READER_TYPE_TEXT;
            atext->name = (const xmlChar *)"text";

            const char * content = strndup((const char *)characters_start, 1);
            atext->content = (xmlChar *)content;
            nodes->push_back(atext);
            ++characters_start;

          }

          // Copy the remainder after (
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

  static const char * atomic[] = { "name", "operator", "literal", "modifier", 0 };

  if((start + 2) >= nodes->size())
    return false;

  if((xmlReaderTypes)nodes->at(start)->type != XML_READER_TYPE_ELEMENT)
    return false;

  if((xmlReaderTypes)nodes->at(start + 2)->type != XML_READER_TYPE_END_ELEMENT)
    return false;

  if(strcmp((const char *)nodes->at(start)->name, (const char *)nodes->at(start + 2)->name) != 0)
    return false;

  for(int i = 0; atomic[i]; ++i)
    if(strcmp((const char *)nodes->at(start)->name, atomic[i]) == 0)
      return true;

  return false;
}

void addNamespace(xmlNsPtr * nsDef, xmlNsPtr ns);
void merge_filename(xmlNodePtr unit_old, xmlNodePtr unit_new);

// create srcdiff unit
xmlNodePtr create_srcdiff_unit(xmlNodePtr unit_old, xmlNodePtr unit_new) {

  // get units from source code
  xmlNodePtr unit = unit_old;

  // add diff namespace
  addNamespace(&unit->nsDef, &diff);

  merge_filename(unit, unit_new);

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

void merge_filename(xmlNodePtr unit_old, xmlNodePtr unit_new) {

  xmlNodePtr unit = unit_old;

  std::string filename_old = "";
  xmlAttrPtr attr;
  for(attr = unit->properties; attr; attr = attr->next)
    if(strcmp((const char *)attr->name, "filename") == 0) {

      filename_old += (const char *)attr->children->content;
      break;
    }

  std::string filename_new = "";
  xmlAttrPtr attr_new;
  for(attr_new = unit_new->properties; attr_new; attr_new = attr_new->next)
    if(strcmp((const char *)attr_new->name, "filename") == 0) {

      filename_new += (const char *)attr_new->children->content;
      break;
    }

  std::string * filename = NULL;
  if(attr && attr_new) {

    if(filename_old == filename_new)
      return;

    filename = new std::string(filename_old + "|" + filename_new);
    attr->children->content = (xmlChar *)filename->c_str();
    return;

  }

  if(attr_new) {
    
    attr = unit->properties;
    if(attr) {
      
      for(; attr->next; attr = attr->next)
        ;
      
      attr->next = attr_new;
      
    } else {
      
      unit->properties = attr_new;
    }
  }

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

void output_nested(reader_state & rbuf_old, std::vector<int> * structure_old
                   , reader_state & rbuf_new ,std::vector<int> * structure_new
                   , int operation, writer_state & wstate) {


  if(operation == DELETE) {

    // may need to markup common that does not output common blocks
    output_white_space_all(rbuf_old, rbuf_new, wstate);
    //markup_whitespace(rbuf_old, structure_old->at(0), rbuf_new, rbuf_new.last_output, wstate);

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
            && ((xmlReaderTypes)nodes_old.at(structure_old->at(start))->type != XML_READER_TYPE_END_ELEMENT
                || strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "condition") != 0); ++start)
        ;

      ++start;

      start_pos = structure_old->at(start) + 1;
      end_pos = structure_old->back();

      if(strcmp((const char *)nodes_old.at(structure_old->at(0))->name, "if") == 0) {

        ++start_pos;
        --end_pos;

      }

    } else {

      for(start = 0; start < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "incr") != 0; ++start)
        ;

      if(!(nodes_old.at(start)->extra & 0x1))
        for(start = 0; start < structure_old->size() && strcmp((const char *)nodes_old.at(structure_old->at(start))->name, "incr") != 0; ++start)
          ;

      start += 3;
      start_pos = structure_old->at(start) + 1;
      end_pos = structure_old->back();

    }

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_start, DELETE, wstate);


    output_change(rbuf_old, start_pos, rbuf_new, rbuf_new.last_output, wstate);
    //for(unsigned int i = rbuf_old.last_output; i < start_pos; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    //rbuf_old.last_output = start_pos;

    // collect subset of nodes
    std::vector<std::vector<int> *> next_node_set_old
      = create_node_set(nodes_old, start_pos
                        , end_pos);

    std::vector<std::vector<int> *> next_node_set_new
      = create_node_set(nodes_new,  structure_new->at(0)
                        , structure_new->back() + 1);

    output_diffs(rbuf_old, &next_node_set_old, rbuf_new, &next_node_set_new, wstate);

    output_white_space_nested(rbuf_old, rbuf_new, DELETE, wstate);
    //markup_whitespace(rbuf_old, end_pos, rbuf_new, rbuf_new.last_output, wstate);

    output_change(rbuf_old,  structure_old->back() + 1, rbuf_new, rbuf_new.last_output, wstate);
    //for(unsigned int i = end_pos; i < (structure_old->back() + 1); ++i)
    //output_node(rbuf_old, rbuf_new, nodes_old.at(i), DELETE, wstate);

    rbuf_old.last_output = structure_old->back() + 1;

    output_white_space_all(rbuf_old, rbuf_new, wstate);

    // output diff tag begin
    //output_node(rbuf_old, rbuf_new, &diff_old_end, DELETE, wstate);

    //rbuf_new.last_output = structure_new->back() + 1;

    //markup_whitespace(rbuf_old, rbuf_old.last_output, rbuf_new, rbuf_new.last_output, wstate);

  } else {

    // output diff tag
    output_node(rbuf_old, rbuf_new, &diff_new_start, INSERT, wstate);

    //for(unsigned int i = begin_new; i < end_new; ++i)
    //output_node(rbuf_old, rbuf_new, nodes_new.at(i), INSERT, wstate);

    // output diff tag begin
    output_node(rbuf_old, rbuf_new, &diff_new_end, INSERT, wstate);

    rbuf_new.last_output = structure_new->back() + 1;

  }
}
