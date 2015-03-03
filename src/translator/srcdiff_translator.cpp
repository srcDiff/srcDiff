/*
  srcdiff_translator.cpp

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

#include <srcdiff_translator.hpp>

// constructor
srcdiff_translator::srcdiff_translator(const std::string & srcdiff_filename, const OPTION_TYPE & flags, const METHOD_TYPE & method, srcml_archive * archive,
                                       const boost::any & bash_view_context, const boost::optional<std::string> & summary_type_str)
  : archive(archive), flags(flags), output(archive, srcdiff_filename, flags, method, bash_view_context, summary_type_str) {}


// destructor
srcdiff_translator::~srcdiff_translator() {

  output.close();

}

