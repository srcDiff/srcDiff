/*
  srcdiff_translator.hpp

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

#ifndef INCLUDED_SRCDIFF_TRANSLATOR_HPP
#define INCLUDED_SRCDIFF_TRANSLATOR_HPP

#include <srcdiff_input.hpp>
#include <srcdiff_output.hpp>
#include <srcdiff_options.hpp>
#include <methods.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>


#include <srcml.h>

class srcdiff_translator {
 public:

  // constructor
  srcdiff_translator(const std::string & srcdiff_filename,
                    OPTION_TYPE & flags, METHOD_TYPE method,
                    srcml_archive * archive,
                    unsigned long number_context_lines);

  void close();

  void translate(srcdiff_input & input_old, srcdiff_input & input_new,
                 LineDiffRange line_diff_range, const std::string & language,
                 const boost::optional<std::string> & unit_directory = boost::optional<std::string>(), const boost::optional<std::string> & unit_filename = boost::optional<std::string>(),
                 const boost::optional<std::string> & unit_version = boost::optional<std::string>());

  srcml_archive * get_archive();

  // destructor
  ~srcdiff_translator();

 private:

  srcml_archive * archive;

  OPTION_TYPE & flags;

  srcdiff_output output;

};

#endif
