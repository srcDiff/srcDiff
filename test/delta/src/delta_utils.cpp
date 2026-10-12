// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file delta_utils.cpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <delta_utils.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <stdexcept>
#include <string.h>
#include <vector>

#ifdef _MSC_BUILD
#define strdup _strdup
#endif

namespace {

const char * INSERT_TAG = "insert";
const char * DELETE_TAG = "delete";
const char * COMMON_TAG = "common";
const char * WS_TAG = "ws";
const char * DIFF_NS_PREFIX = "diff";
const char * DIFF_NS_URL = "http://www.srcML.org/srcDiff";

const char * XML_VERSION = "1.0";
const char * output_encoding = "UTF-8";
const char * XML_DECLARATION_STANDALONE = "yes";

const char * TYPE_ATTR = "type";
const char * REPLACE_ATTR_VALUE = "replace";

bool check_diff_tag(xmlNodePtr node, const char * tag_name) {
    return (
        node->type == XML_ELEMENT_NODE &&
        xmlStrcmp(node->ns->prefix, BAD_CAST DIFF_NS_PREFIX) == 0 &&
        xmlStrcmp(node->name, BAD_CAST tag_name) == 0
    );
}

void get_surviving_descendants(
    xmlNodePtr descendants, const char * allowed_tag, std::vector<xmlNodePtr>& survivors
) {
    for (xmlNodePtr cur = descendants; cur != NULL; cur = cur->next) {
        if (check_diff_tag(cur, allowed_tag) || check_diff_tag(cur, COMMON_TAG)) {
                for (xmlNodePtr child = cur->children; child != NULL; child = child->next) {
                    survivors.push_back(child);
                }
        } else if (cur->children) {
            get_surviving_descendants(cur->children, allowed_tag, survivors);
        }
    }
}

void get_srcml_for_revision(xmlNodePtr node, int revision) {
    const char * allowed_tag = (revision == 0) ? DELETE_TAG : INSERT_TAG;
    const char * disallowed_tag = (revision == 1) ? DELETE_TAG : INSERT_TAG;
    xmlNodePtr next;
    for (xmlNodePtr cur = node; cur != NULL; cur = next) {
        if (check_diff_tag(cur, disallowed_tag)) {
            std::vector<xmlNodePtr> survivors;
            get_surviving_descendants(cur->children, allowed_tag, survivors);
            xmlNodePtr first_new_node = NULL;
            // add as previous siblings to keep their order
            for (xmlNodePtr survivor : survivors) {
                xmlNodePtr new_node = xmlAddPrevSibling(cur, survivor);
                if (!first_new_node) {
                    first_new_node = new_node;
                }
            }
            if (first_new_node) {
                next = first_new_node;
            } else {
                next = cur->next;
            }
            xmlUnlinkNode(cur);
            xmlFreeNode(cur);
        } else if (check_diff_tag(cur, WS_TAG) ||
                    check_diff_tag(cur, COMMON_TAG) ||
                    check_diff_tag(cur, allowed_tag)) {
            xmlNodePtr first_new_node = NULL;
            xmlNodePtr nextChild;
            for (xmlNodePtr child = cur->children; child != NULL; child = nextChild) {
                nextChild = child->next;
                xmlNodePtr new_node = xmlAddPrevSibling(cur, child);
                if (!first_new_node) {
                    first_new_node = new_node;
                }
            }
            if (first_new_node) {
                next = first_new_node;
            } else {
                next = cur->next;
            }
            xmlUnlinkNode(cur);
            xmlFreeNode(cur);
        } else {
            next = cur->next;
            if (cur->children) {
                get_srcml_for_revision(cur->children, revision);
            }
        }
    }
}

std::string get_all_node_text(xmlNodePtr node) {
    std::string result = "";
    for (xmlNodePtr cur = node; cur != NULL; cur = cur->next) {
        if (cur->content) {
            result += (char *)cur->content;
        }
        if (cur->children) {
            result += get_all_node_text(cur->children);
        }
    }
    return result;
}

inline bool isendelement(const xmlTextReaderPtr& reader) {
  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT;
}

void outputNode(const xmlNode & node, xmlTextWriterPtr writer, bool output_ns) {

  bool isemptyelement = false;

  switch ((xmlReaderTypes)node.type) {
  case XML_READER_TYPE_ELEMENT:

    isemptyelement = node.extra & 0x1;

    if (node.ns && node.ns->prefix) {

      std::string s = ((char*) node.ns->prefix);
      s += ":";
      s += (char*) node.name;

      xmlTextWriterStartElement(writer, BAD_CAST s.c_str());

    } else

      xmlTextWriterStartElement(writer, (xmlChar *)node.name);

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

const char * get_attr(xmlNodePtr node, const char * attribute) {

  for(xmlAttrPtr attr = node->properties; attr; attr = attr->next) {
    if(strcmp((const char *)attr->name, attribute) == 0)
      return (const char *)attr->children->content;
  }

  return 0;
}

}

std::string serialize_unit(srcml_unit* unit) {

    srcml_archive * out_archive = srcml_archive_create();
    srcml_archive_enable_solitary_unit(out_archive);
    srcml_archive_register_namespace(out_archive, DIFF_NS_PREFIX, DIFF_NS_URL);

    char * archive_out_chars = nullptr;
    size_t archive_out_size = 0;
    srcml_archive_write_open_memory(out_archive, &archive_out_chars, &archive_out_size);
    int status = srcml_archive_write_unit(out_archive, unit);
    srcml_archive_close(out_archive);
    srcml_archive_free(out_archive);

    if (status != SRCML_STATUS_OK) {
        srcml_memory_free(archive_out_chars);
        throw std::runtime_error("Could not serialize unit with libsrcml");
    }

    std::string result(archive_out_chars, archive_out_size);
    srcml_memory_free(archive_out_chars);

    return result;
}

std::string extract_revision_source(const std::string& srcdiff, int revision) {

    xmlDocPtr doc = xmlReadMemory(srcdiff.c_str(), (int)srcdiff.size(), nullptr, "UTF-8", 0);
    if (doc == NULL) {
        throw std::runtime_error("Could not parse srcDiff unit as XML");
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    get_srcml_for_revision(root->children, revision);
    std::string source = get_all_node_text(root->children);

    xmlFreeDoc(doc);

    return source;
}

std::string switch_differences(const std::string& srcdiff) {

  xmlTextReaderPtr reader = xmlReaderForMemory(srcdiff.c_str(), (int)srcdiff.size(), nullptr, nullptr, 0);
  if (reader == NULL) {
    throw std::runtime_error("Unable to read srcDiff unit as XML");
  }

  xmlBufferPtr buffer = xmlBufferCreate();
  xmlTextWriterPtr writer = xmlNewTextWriterMemory(buffer, 0);
  if (writer == NULL) {
    xmlFreeTextReader(reader);
    xmlBufferFree(buffer);
    throw std::runtime_error("Unable to create XML writer");
  }

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

    if(node->ns && node->ns->prefix && strcmp((const char *)node->ns->prefix, DIFF_NS_PREFIX) == 0) {

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

  std::string result((const char *)xmlBufferContent(buffer), xmlBufferLength(buffer));
  xmlBufferFree(buffer);

  return result;
}
