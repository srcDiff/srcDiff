/*
  Options.hpp

  Copyright (C) 2005-2014  SDML (www.srcML.org)

  This file is part of the srcML Toolkit.

  The srcML Toolkit is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML Toolkit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML Toolkit; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
  Options for translation output.
*/

#ifndef INCLUDED_OPTION_HPP
#define INCLUDED_OPTION_HPP

#ifdef __GNUC__
typedef unsigned long long OPTION_TYPE;
#else
typedef unsigned __int64 OPTION_TYPE;
#endif

#ifdef __GNUC__
#define ull(a) a##ULL
#else
#define ull(a) a##i64
#endif

inline bool isoption(OPTION_TYPE options, OPTION_TYPE flag) {
    return (flag & options) > 0;
}

// begin SRCML_OPTIONS

// unit is nested a root unit
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_ARCHIVE      = ull(1) << __COUNTER__; /* 0 */

// markup literal values
// @src2srcml
const OPTION_TYPE OPTION_LITERAL     = ull(1) << __COUNTER__; /* 1 */

// markup type modifiers
// @src2srcml
const OPTION_TYPE OPTION_MODIFIER     = ull(1) << __COUNTER__; /* 2 */

// markup operators
// @src2srcml
const OPTION_TYPE OPTION_OPERATOR     = ull(1) << __COUNTER__; /* 3 */

// issue debugging elements and declare debugging element namespace
// @src2srcml
const OPTION_TYPE OPTION_DEBUG       = ull(1) << __COUNTER__; /* 4 */

// position
// @src2srcml
const OPTION_TYPE OPTION_POSITION    = ull(1) << __COUNTER__; /* 5 */

// cpp mode
// @src2srcml
const OPTION_TYPE OPTION_CPP_NOMACRO = ull(1) << __COUNTER__; /* 6 */
const OPTION_TYPE OPTION_CPP = ull(1) << __COUNTER__ | OPTION_CPP_NOMACRO; /* 7 */

// xml declaration
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_XMLDECL = ull(1) << __COUNTER__; /* 8 */

// namespace declaration
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_NAMESPACEDECL = ull(1) << __COUNTER__; /* 9 */

// cpp markup #else sections
// @src2srcml
const OPTION_TYPE OPTION_CPP_TEXT_ELSE = ull(1) << __COUNTER__; /* 10 */

// cpp markup #if 0 sections
// @src2srcml
const OPTION_TYPE OPTION_CPP_MARKUP_IF0 = ull(1) << __COUNTER__; /* 11 */

// xslt all evaluation
// @srcml2src
const OPTION_TYPE OPTION_APPLY_ROOT      = ull(1) << __COUNTER__; /* 12 */

// output is compressed (gzip)
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_COMPRESSED  = ull(1) << __COUNTER__; /* 13 */

// @src2srcml
const OPTION_TYPE OPTION_NESTIF  = ull(1) << __COUNTER__; /* 14 */

// end SRCML_OPTIONS

// path
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_PATH        = ull(1) << __COUNTER__;

// language
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_LANGUAGE    = ull(1) << __COUNTER__;

// xml encoding
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_XML_ENCODING= ull(1) << __COUNTER__;

// output is OPTION_TYPEeractive (good for editing applications) @src2srcml
// @src2srcml
const OPTION_TYPE OPTION_INTERACTIVE = ull(1) << __COUNTER__;

// input filenames is a file list
// @src2srcml
const OPTION_TYPE OPTION_FILELIST    = ull(1) << __COUNTER__;

// declared filename
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_FILENAME    = ull(1) << __COUNTER__;

// declared directory
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_DIRECTORY   = ull(1) << __COUNTER__;

// declared version
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_VERSION     = ull(1) << __COUNTER__;

// verbose translation
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_VERBOSE     = ull(1) << __COUNTER__;

// language
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_SRC_ENCODING= ull(1) << __COUNTER__;

// text extraction
// @srcml2src
const OPTION_TYPE OPTION_TEXT         = ull(1) << __COUNTER__;

// language
// @srcml2src
const OPTION_TYPE OPTION_UNIT         = OPTION_OPERATOR;

// expand nested units OPTION_TYPEo separate files
// @srcml2src
const OPTION_TYPE OPTION_TO_DIR       = OPTION_MODIFIER;

// output is xml
// @srcml2src
const OPTION_TYPE OPTION_XML          = ull(1) << __COUNTER__;

// info
// @srcml2src
const OPTION_TYPE OPTION_INFO         = ull(1) << __COUNTER__;

// long info
// @srcml2src
const OPTION_TYPE OPTION_LONG_INFO    = ull(1) << __COUNTER__;

const OPTION_TYPE OPTION_XPATH_TOTAL = OPTION_CPP_TEXT_ELSE;

// terminate after this file
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_TERMINATE = ull(1) << __COUNTER__;

// expression mode
// @src2srcml
const OPTION_TYPE OPTION_EXPRESSION = ull(1) << __COUNTER__;

// namespace mode
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_NAMESPACE = ull(1) << __COUNTER__;

// omit namespace mode
// @srcml2src
const OPTION_TYPE OPTION_OMIT = ull(1) << __COUNTER__;

// xmlns mode
// @src2srcml
const OPTION_TYPE OPTION_XMLNS = OPTION_APPLY_ROOT;

// self version
//  const OPTION_TYPE OPTION_SELF_VERSION = 0x02000000;


// xpath evaluation
// @srcml2src
const OPTION_TYPE OPTION_XPATH        = OPTION_LITERAL;

// xslt evaluation
// @srcml2src
const OPTION_TYPE OPTION_XSLT         = OPTION_CPP;

// relaxng evaluation
// @srcml2src
const OPTION_TYPE OPTION_RELAXNG      = OPTION_EXPRESSION;

// use old filename behavior, where it was just the base filename
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_OLD_FILENAME  = ull(1) << __COUNTER__;

// specify input format
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_INPUT_FORMAT  = ull(1) << __COUNTER__;

// specify ouput format
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_OUTPUT_FORMAT  = ull(1) << __COUNTER__;

// list option
// @srcml2src
const OPTION_TYPE OPTION_LIST  = ull(1) << __COUNTER__;

// skip default language option
// @src2srcml
const OPTION_TYPE OPTION_RECURSIVE  = ull(1) << __COUNTER__;

// skip output of notification messages
// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_QUIET  = ull(1) << __COUNTER__;

// @srcml2src
const OPTION_TYPE OPTION_DIFF  = ull(1) << __COUNTER__;

// @srcml2src
const OPTION_TYPE OPTION_PRESERVE  = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_REVISION  = ull(1) << __COUNTER__;

// @src2srcml
// @srcml2src
const OPTION_TYPE OPTION_NULL  = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_SVN  = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_LINE  = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_MACRO_PATTERN  = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_MACRO_LIST  = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_CPPIF_CHECK  = ull(1) << __COUNTER__;

#endif
