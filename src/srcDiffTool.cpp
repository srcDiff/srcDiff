/*
  srcDiffTool.cpp

  Copyright (C) 2011  SDML (www.sdml.info)

  This file is part of the srcDiff translator.

  The srcDiff translator is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcDiff translator is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcDiff translator; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Class for straightforward translation from source code to srcDiff
*/

#include "srcDiffTool.hpp"
#include "srcmlns.hpp"
#include "srcmlapps.hpp"

// constructor
srcDiffTool::srcDiffTool(int language, const char* srcml_filename, OPTION_TYPE& op)
  : 
  srcMLOutput(0, srcml_filename, "", DEFAULT_XML_ENCODING, op, (const char**)DEFAULT_URI_PREFIX, 8, 0),
    first(true),
    root_directory(""), root_filename(""), root_version(""),
    encoding(DEFAULT_TEXT_ENCODING), options(op)
{
}

// constructor
srcDiffTool::srcDiffTool(int language,                // programming language of source code
				 const char* src_encoding,    // text encoding of source code
				 const char* xml_encoding,    // xml encoding of result srcML file
				 const char* srcml_filename,  // filename of result srcML file
				 OPTION_TYPE& op,             // many and varied options
				 const char* directory,       // root unit directory
				 const char* filename,        // root unit filename
				 const char* version,         // root unit version
				 const char* uri[],           // uri prefixes
				 int tabsize                  // size of tabs
				 )
  : srcMLOutput(0, srcml_filename, "", xml_encoding, op, uri, tabsize, 0),
    first(true),
    root_directory(directory), root_filename(filename), root_version(version),
    encoding(src_encoding), options(op)
{
}

// close the output
void srcDiffTool::close() {

  close();
}

// translate from input stream to output stream
void srcDiffTool::translate(const char* path, const char* unit_directory,
				const char* unit_filename, const char* unit_version,
				int language) {

  // root unit for compound srcML documents
  if (first && ((options & OPTION_NESTED) > 0))
    startUnit(0, root_directory, root_filename, root_version, true);

  first = false;

}

// destructor
srcDiffTool::~srcDiffTool() {
}
