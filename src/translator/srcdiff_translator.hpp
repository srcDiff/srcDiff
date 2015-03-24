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

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include <string>

#include <srcml.h>

class srcdiff_translator {

private:

  srcml_archive * archive;

  const OPTION_TYPE & flags;

  srcdiff_output output;

public:

  // constructor
  srcdiff_translator(const std::string & srcdiff_filename, const OPTION_TYPE & flags, const METHOD_TYPE & method, srcml_archive * archive,
                    const boost::any & bash_view_context, const boost::optional<std::string> & summary_type_str);

  // destructor
  ~srcdiff_translator();

  template<class T>
  void translate(const srcdiff_input<T> & input_original, const srcdiff_input<T> & input_modified,
                 line_diff_range<T> & line_diff_range, const std::string & language,
                 const boost::optional<std::string> & unit_directory = boost::optional<std::string>(),
                 const boost::optional<std::string> & unit_filename  = boost::optional<std::string>(),
                 const boost::optional<std::string> & unit_version   = boost::optional<std::string>());

};

#include <thread>
#include <srcdiff_diff.hpp>
#include <srcdiff_whitespace.hpp>

// Translate from input stream to output stream
template<class T>
void srcdiff_translator::translate(const srcdiff_input<T> & input_original, const srcdiff_input<T> & input_modified,
                                  line_diff_range<T> & line_diff_range, const std::string & language,
                                  const boost::optional<std::string> & unit_directory,
                                  const boost::optional<std::string> & unit_filename,
                                  const boost::optional<std::string> & unit_version) {

  // line_diff_range.create_line_diff();

  // if(!is_option(flags, OPTION_SAME) && line_diff_range.get_line_diff() == NULL)
  //   return;

  int is_original = 0;
  std::thread thread_original(std::ref(input_original), SESDELETE, std::ref(output.get_nodes_original()), std::ref(is_original));

  int is_modified = 0;
  std::thread thread_modified(std::ref(input_modified), SESINSERT, std::ref(output.get_nodes_modified()), std::ref(is_modified));

  thread_original.join();
  thread_modified.join();

  node_sets set_original(output.get_nodes_original(), 0, output.get_nodes_original().size());
  node_sets set_modified(output.get_nodes_modified(), 0, output.get_nodes_modified().size());

  output.initialize(is_original, is_modified);

  // run on file level
  if(is_original || is_modified) {

    output.start_unit(language, unit_directory, unit_filename, unit_version);

    srcdiff_diff diff(output, set_original, set_modified);
    diff.output();

    // output remaining whitespace
    srcdiff_whitespace whitespace(output);
    whitespace.output_all();

    output.finish(line_diff_range);

  }

  output.reset();

}

#endif
