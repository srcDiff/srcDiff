#ifndef __INCLUDED_DIFFRW_H
#define __INCLUDED_DIFFRW_H

#include <cstring>

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

inline bool isediffstart(xmlTextReaderPtr reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && 
         (strcmp((const char*) xmlTextReaderConstName(reader), DIFF_OLD) == 0 ||
	  strcmp((const char*) xmlTextReaderConstName(reader), DIFF_NEW) == 0) &&
	  strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_BEGIN) == 0;
}

inline bool isediffend(xmlTextReaderPtr reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && 
         (strcmp((const char*) xmlTextReaderConstName(reader), DIFF_OLD) == 0 ||
	  strcmp((const char*) xmlTextReaderConstName(reader), DIFF_NEW) == 0) &&
	  strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_END) == 0;
}

inline bool isediffoldstart(xmlTextReaderPtr reader) {

  return strcmp((const char*) xmlTextReaderConstName(reader), DIFF_OLD) == 0 &&
	 strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_BEGIN) == 0;
}

inline bool isediffoldend(xmlTextReaderPtr reader) {

  return strcmp((const char*) xmlTextReaderConstName(reader), DIFF_OLD) == 0 &&
	 strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_END) == 0;
}

inline bool isediffnewstart(xmlTextReaderPtr reader) {

  return strcmp((const char*) xmlTextReaderConstName(reader), DIFF_NEW) == 0 &&
	 strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_BEGIN) == 0;
}

inline bool isediffnewend(xmlTextReaderPtr reader) {

  return strcmp((const char*) xmlTextReaderConstName(reader), DIFF_NEW) == 0 &&
	 strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_END) == 0;
}

inline bool isediffcommonstart(xmlTextReaderPtr reader) {

  return strcmp((const char*) xmlTextReaderConstName(reader), DIFF_COMMON) == 0 &&
	 strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_BEGIN) == 0;
}

inline bool isediffcommonend(xmlTextReaderPtr reader) {

  return strcmp((const char*) xmlTextReaderConstName(reader), DIFF_COMMON) == 0 &&
	 strcmp((const char*) xmlTextReaderGetAttribute(reader, EDIFF_ATTRIBUTE), EDIFF_END) == 0;
}

inline bool isediff(xmlTextReaderPtr reader) {

  return xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT && 
         (strcmp((const char*) xmlTextReaderConstName(reader), DIFF_OLD) == 0 ||
	  strcmp((const char*) xmlTextReaderConstName(reader), DIFF_NEW) == 0);
}

#endif


