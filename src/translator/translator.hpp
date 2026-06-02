// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file translator.hpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_TRANSLATOR_HPP
#define INCLUDED_TRANSLATOR_HPP

#include <input_stream_manager.hpp>
#include <input_stream.hpp>
#include <output_stream.hpp>
#include <client_options.hpp>
#include <differ.hpp>
#include <whitespace_stream.hpp>
#include <methods.hpp>

#include <unit.hpp>

#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>

#include <any>
#include <optional>
#include <string>

#include <srcml.h>

namespace srcdiff {

class translator {

private:

  srcml_archive * archive;

  const OPTION_TYPE & flags;

  std::shared_ptr<output_stream> output;

  const std::optional<std::string> & unit_filename;

public:

  // constructor
  translator(const std::string& srcdiff_filename, const OPTION_TYPE& flags, const METHOD_TYPE& method, srcml_archive * archive,
             const std::optional<std::string>& unit_filename,
             const client_options::view_options_t& view_options,
             const std::optional<std::string>& summary_type_str);

  // destructor
  ~translator();

  template<class T>
  std::string translate(
                 const input_stream<T> & input_original,
                 const input_stream<T> & input_modified,
                 const std::string & language,
                 const std::optional<std::string> & unit_filename  = std::optional<std::string>(),
                 const std::optional<std::string> & unit_version   = std::optional<std::string>());

  // Think about taking out archive writing and putting having in input
  // separates concerns better
  void write_translation();
};

// Translate from input stream to output stream
template<class T>
std::string translator::translate( const input_stream<T> & input_original,
                                   const input_stream<T> & input_modified,
                                   const std::string & language,
                                   const std::optional<std::string> & unit_filename,
                                   const std::optional<std::string> & unit_version) {

  const std::optional<std::string> output_path = "";

  input_stream_manager input_manager(archive, flags);
  input_manager.append_stream(input_original);
  input_manager.append_stream(input_modified);
  std::tie(output->nodes_original(), output->nodes_modified()) = input_manager.consume_streams();

  output->prime();

  std::string srcdiff_str;
  // run on file level
  if(!output->nodes_original().empty() || !output->nodes_modified().empty()) {

    output->start_unit(language, this->unit_filename ? this->unit_filename : unit_filename, unit_version);

    unit original_unit(output->nodes_original(), output);
    unit modified_unit(output->nodes_modified(), output);

    differ diff(output, original_unit.children(), modified_unit.children());
    diff.output();

    // output remaining whitespace
    whitespace_stream whitespace(*output);
    whitespace.output_all();

    srcdiff_str = output->end_unit();

  }

  output->reset();

  return srcdiff_str;
}

}

#endif
