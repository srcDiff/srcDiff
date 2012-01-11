#ifndef INCLUDED_SRCDIFFCONSTANTS_HPP
#define INCLUDED_SRCDIFFCONSTANTS_HPP

const char * const DIFF_PREFIX = "diff:";
const char * const DIFF_OLD = "delete";
const char * const DIFF_NEW = "insert";
const char * const DIFF_SESCOMMON = "common";
const char * const DIFF_TYPE = "type";

const char * const change = "change";
const char * const whitespace = "whitespace";

// special flush node
const xNode flush = { (xmlElementType)XML_READER_TYPE_TEXT, "text", 0, "", 0, 0, true, false };

#endif
