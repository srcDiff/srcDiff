// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file switch_differences.cpp
 *
 * @copyright Copyright (C) 2011-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

// unused parameters, unsigned vs signed comparison
const char * XML_VERSION = "1.0";
const char * output_encoding = "UTF-8";
const char * XML_DECLARATION_STANDALONE = "yes";

#ifdef _MSC_BUILD

#define strdup _strdup

char * strndup(const char * source, size_t n) {

  if(source == 0) return 0;

  char * dup = (char *)malloc((n + 1) * sizeof(char));
  strncpy_s(dup, n+1, source, n);
  dup[n] = 0;

  return dup;

}

#endif

inline bool iselement(const xmlTextReaderPtr& reader, const xmlChar* element_name) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT &&
    xmlStrEqual(xmlTextReaderConstName(reader), element_name);
}

inline bool isendelement(const xmlTextReaderPtr& reader, const xmlChar* element_name) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT &&
    xmlStrEqual(xmlTextReaderConstName(reader), element_name);
}

inline bool iselement(const xmlTextReaderPtr& reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT;
}

inline bool isendelement(const xmlTextReaderPtr& reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT;
}

// output current XML node in reader
void outputNode(const xmlNode & node, xmlTextWriterPtr writer, bool output_ns) {

  bool isemptyelement = false;

  switch ((xmlReaderTypes)node.type) {
  case XML_READER_TYPE_ELEMENT:

    // record if this is an empty element since it will be erased by the attribute copying
    isemptyelement = node.extra & 0x1;

    // start the element
    if (node.ns && node.ns->prefix) {
      std::string s = ((char*) node.ns->prefix);
      s += ":";
      s += (char*) node.name;

      xmlTextWriterStartElement(writer, BAD_CAST s.c_str());

    } else
      xmlTextWriterStartElement(writer, (xmlChar *)node.name);

    // copy all the namespaces

    if(output_ns){
      xmlNs * ns = node.nsDef;
      while (ns) {

        std::string ns_name = "xmlns";
        if(ns->prefix) {

          ns_name += ":";
          ns_name += (const char *)ns->prefix;

        }

        xmlTextWriterWriteAttribute(writer, (const xmlChar *)ns_name.c_str(), (const xmlChar *)ns->href);
        ns = ns->next;
      }
    }

    // copy all the attributes
    {
      xmlAttr * attribute = node.properties;
      while (attribute) {
        std::string value = (const char *)attribute->children->content;
        std::string::size_type pos = value.find('|');
        if(pos != std::string::npos) {

          std::string temp = value.substr(pos + 1);
          temp += "|";
          temp += value.substr(0, pos);

          value = temp;

        }

        xmlTextWriterWriteAttribute(writer, (const xmlChar *)attribute->name, (const xmlChar *)value.c_str());
        attribute = attribute->next;
      }
    }

    // end now if this is an empty element
    if (isemptyelement) {

      xmlTextWriterEndElement(writer);
    }

    break;

  case XML_READER_TYPE_END_ELEMENT:
    xmlTextWriterEndElement(writer);
    break;

  case XML_READER_TYPE_COMMENT:
    xmlTextWriterWriteComment(writer, (const xmlChar *)node.content);
    break;

  case XML_READER_TYPE_TEXT:
  case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:

    // output the UTF-8 buffer escaping the characters.  Note that the output encoding                                                                                                                                                                                                                                                      
    // is handled by libxml                                                                                                                                                                                                                                                                                                                 
    for (unsigned char* p = (unsigned char*) node.content; *p != 0; ++p) {
      if (*p == '&') {
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&amp;", 5);
      }
      else if (*p == '<') {
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&lt;", 4);
      }
      else if (*p == '>') {
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) "&gt;", 4);
      }
      else {
        xmlTextWriterWriteRawLen(writer, BAD_CAST (unsigned char*) p, 1);
      }
    }
    break;

  default:
    break;
  }
}

// macros
#define SIZEPLUSLITERAL(s) sizeof(s) - 1, BAD_CAST s
#define LITERALPLUSSIZE(s) BAD_CAST s, sizeof(s) - 1

const char * const DIFF_PREFIX = "diff";
const char * const DELETE_TAG = "delete";
const char * const INSERT_TAG = "insert";
const char * const TYPE_ATTR = "type";
const char * const REPLACE_ATTR_VALUE = "replace";

const char * get_attr(xmlNodePtr node, const char * attribute) {

  for(xmlAttrPtr attr = node->properties; attr; attr = attr->next) {
    if(strcmp((const char *)attr->name, attribute) == 0)
      return (const char *)attr->children->content;
  }

  return 0;

}

// are parameters necessary here?
int main(/*int argc, char * argv[]*/) {

  /*
    Create xmlreader and the xmlwriter
  */

  xmlTextReaderPtr reader = NULL;

  xmlTextWriterPtr writer = NULL;

  // create the reader for the old file
  reader = xmlNewTextReaderFilename("-");
  if (reader == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", "-");

    return 1;
  }

  // create the writer
  writer = xmlNewTextWriterFilename("-", 0);
  if (writer == NULL) {
    fprintf(stderr, "Unable to open file '%s' as XML", "-");

    return 1;
  }

  // issue the xml declaration
  xmlTextWriterStartDocument(writer, XML_VERSION, output_encoding, XML_DECLARATION_STANDALONE);

  bool output_ns = true;
  bool is_change = false;
  bool wait_end = false;
  bool output_saved = false;
  std::vector<xmlNodePtr> nodes;
  int change_depth = 0;
  while(xmlTextReaderRead(reader) == 1) {

    xmlNodePtr node = xmlCopyNode(xmlTextReaderCurrentNode(reader), 2);
    node->type = (xmlElementType)xmlTextReaderNodeType(reader);
    node->extra = xmlTextReaderIsEmptyElement(reader);

    if(node->ns && node->ns->prefix && strcmp((const char *)node->ns->prefix, DIFF_PREFIX) == 0) {

      if(strcmp((const char *)node->name, DELETE_TAG) == 0) {

        node->name = (const xmlChar *)strdup(INSERT_TAG);

        if(!isendelement(reader) && get_attr(node, TYPE_ATTR) && strcmp(get_attr(node, TYPE_ATTR), REPLACE_ATTR_VALUE) == 0) {
          is_change = true;
        }

      } else if(strcmp((const char *)node->name, INSERT_TAG) == 0) {

        node->name = (const xmlChar *)strdup(DELETE_TAG);

        if(is_change && !isendelement(reader) && get_attr(node, TYPE_ATTR) && strcmp(get_attr(node, TYPE_ATTR), REPLACE_ATTR_VALUE) == 0) {

          is_change = false;
          wait_end = true;

        }

        if(wait_end && isendelement(reader)) {
          --change_depth;
        }
        else if(wait_end) {
          ++change_depth;
        }

        if(wait_end && isendelement(reader) && change_depth == 0) {

          wait_end = false;
          output_saved = true;

        }

      }

    }

    if(is_change) {
      nodes.push_back(node);
    }
    else {

      outputNode(*node, writer, output_ns);

    }

    if(output_saved) {

      for(std::size_t i = 0; i < nodes.size(); ++i) {

        outputNode(*nodes[i], writer, output_ns);

      }

      output_saved = false;
      nodes.clear();

    }

    output_ns = false;

  }

  xmlFreeTextReader(reader);

  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);

  return 0;
}

