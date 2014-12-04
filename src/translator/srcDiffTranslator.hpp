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

#include <Options.hpp>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <srcDiffTypes.hpp>
#include <Methods.hpp>
#include <ColorDiff.hpp>
#include <bash_view.hpp>

#include <pthread.h>
#include <srcml.h>

class srcDiffTranslator {
 public:

  // constructor
  srcDiffTranslator(const char* srcml_filename,
                    METHOD_TYPE method,
                    std::string css,
                    srcml_archive * archive,
                    const char * url,
                    OPTION_TYPE & options,
                    unsigned long number_context_lines);

  void close();

  void translate(const char* path_one, const char* path_two, const char* unit_directory = 0, const char* unit_filename = 0, const char* unit_version = 0);

  srcml_archive * get_archive();

  // destructor
  ~srcDiffTranslator();

 private:

  METHOD_TYPE method;
  srcml_archive * archive;

  pthread_mutex_t mutex;
  reader_state rbuf_old;
  reader_state rbuf_new;

  writer_state wstate;

  ColorDiff * colordiff;
  bash_view * bashview;

  const char * url;

  OPTION_TYPE & options;

};

#endif
