/*
  srcDiffTool.hpp

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
*/

/*
  Class for straightforward source code to srcDiff translation
*/

#ifndef INCLUDED_SRCDIFFTRANSLATOR_HPP
#define INCLUDED_SRCDIFFTRANSLATOR_HPP

#include <Language.hpp>
#include <Options.hpp>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include "srcDiffTypes.hpp"
#include "Methods.hpp"
#include "ColorDiff.hpp"

class srcDiffTranslator {
 public:

  // constructor
  srcDiffTranslator(int language,
		  const char* src_encoding,
		  const char* xml_encoding,
		  const char* srcml_filename,
		  OPTION_TYPE global_options,
		  METHOD_TYPE method,
		  const char* directory,
		  const char* filename,
		  const char* version,
		  const char* uri[],
		  int tabsize
		  );

  void close();

  void translate(const char* path_one, const char* path_two, OPTION_TYPE srcml_option,
               const char* unit_directory = 0, const char* unit_filename = 0, const char* unit_version = 0,
               int language = 0);

void startUnit(const char * language,
               OPTION_TYPE& options,        // many and varied options
               const char* directory,       // root unit directory
               const char* filename,        // root unit filename
               const char* version         // root unit version
               );

void outputNamespaces(const OPTION_TYPE& options);

  // destructor
  ~srcDiffTranslator();

  void set_nested(bool is_nested = true);

  void set_root_directory(const char * root_directory);

 private:

  bool first;
  const char* root_directory;
  const char* root_filename;
  const char* root_version;
  const char* src_encoding;
  const char* xml_encoding;
  int language;
  OPTION_TYPE global_options;
  METHOD_TYPE method;
  const char** uri;
  int tabsize;

  reader_state rbuf_old;
  reader_state rbuf_new;

  writer_state wstate;

  xmlBuffer * output_srcml_file_old;
  xmlBuffer * output_srcml_file_new;

  ColorDiff * colordiff;

  //xmlTextWriterPtr writer;

};

#endif
