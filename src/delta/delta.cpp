// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file delta.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <delta.hpp>

namespace srcdiff {

// constructor
/// @todo remove srcdiff filename and in general archive open/close from output stream
delta::delta(const METHOD_TYPE& method, const std::optional<std::string>& unit_filename)
  : output(std::make_shared<srcdiff::output_stream>(method)),
    unit_filename(unit_filename) {}

// destructor
delta::~delta() {
  output->close();
}

// Translate from input stream to output stream
srcml_unit* delta::create(srcml_archive* archive, input_stream_manager& manager,
                          const std::string& language,
                          const std::optional<std::string>& unit_filename,
                          const std::optional<std::string>& unit_version) {

  const std::optional<std::string> output_path = "";

  std::tie(output->nodes_original(), output->nodes_modified()) = manager.consume_streams();

  output->prime();

  std::string srcdiff_str;
  // run on file level
  if(!output->nodes_original().empty() || !output->nodes_modified().empty()) {

    output->start_unit(archive, language, this->unit_filename ? this->unit_filename : unit_filename, unit_version);

    unit original_unit(output->nodes_original(), output);
    unit modified_unit(output->nodes_modified(), output);

    differ diff(output, original_unit.children(), modified_unit.children());
    diff.output();

    // output remaining whitespace
    whitespace_stream whitespace(*output);
    whitespace.output_all();

    output->end_unit();

  }

  output->reset();

  return output->get_unit();
}

}
