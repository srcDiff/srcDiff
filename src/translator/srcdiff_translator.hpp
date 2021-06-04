/*
  srcdiff_translator.hpp

  Copyright (C) 2011-2021  SDML (www.srcML.org)

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
#ifdef TIMING
#include <boost/timer/timer.hpp>
#endif

#include <string>

#include <srcml.h>

class srcdiff_translator {

private:

  srcml_archive * archive;

  const OPTION_TYPE & flags;

  srcdiff_output output;

  const boost::optional<std::string> & unit_filename;

public:

  // constructor
  srcdiff_translator(const std::string & srcdiff_filename, const OPTION_TYPE & flags, const METHOD_TYPE & method, srcml_archive * archive,
                     const boost::optional<std::string> & unit_filename,
                     const srcdiff_options::view_options_t & view_options,
                     const boost::optional<std::string> & summary_type_str);

  // destructor
  ~srcdiff_translator();

  template<class T>
  void translate(const srcdiff_input<T> & input_original,
                 const srcdiff_input<T> & input_modified,
                 line_diff_range<T> & line_diff_range, const std::string & language,
                 const boost::optional<std::string> & unit_filename  = boost::optional<std::string>(),
                 const boost::optional<std::string> & unit_version   = boost::optional<std::string>());

};

#include <thread>
#include <srcdiff_diff.hpp>
#include <srcdiff_whitespace.hpp>

// Translate from input stream to output stream
template<class T>
void srcdiff_translator::translate(const srcdiff_input<T> & input_original,
                                   const srcdiff_input<T> & input_modified,
                                   line_diff_range<T> & line_diff_range,
                                   const std::string & language,
                                   const boost::optional<std::string> & unit_filename,
                                   const boost::optional<std::string> & unit_version) {

  const boost::optional<std::string> output_path = is_option(flags, OPTION_BURST) && is_option(flags, OPTION_SRCML) ? output.srcdiff_filename() : boost::optional<std::string>();

  const srcml_converter::srcml_burst_config burst_config = { output_path, language, (this->unit_filename ? this->unit_filename : unit_filename), unit_version };
  int is_original = 0;
  std::thread thread_original(std::ref(input_original), SES_DELETE, std::ref(output.nodes_original()), std::ref(is_original), burst_config);

  thread_original.join();

  int is_modified = 0;
  std::thread thread_modified(std::ref(input_modified), SES_INSERT, std::ref(output.nodes_modified()), std::ref(is_modified), burst_config);

  thread_modified.join();

#ifdef TIMING
{
    boost::timer::auto_cpu_timer t;
#endif

  node_sets set_original(output.nodes_original(), 0, output.nodes_original().size());
  node_sets set_modified(output.nodes_modified(), 0, output.nodes_modified().size());

  output.initialize(is_original, is_modified);

  // run on file level
  if(is_original || is_modified) {

    output.start_unit(language, this->unit_filename ? this->unit_filename : unit_filename, unit_version);

    srcdiff_diff diff(output, set_original, set_modified);
    diff.output();

    // output remaining whitespace
    srcdiff_whitespace whitespace(output);
    whitespace.output_all();

    output.finish(line_diff_range);

  }

#ifdef TIMING
}
#endif

  output.reset();

}

#endif
