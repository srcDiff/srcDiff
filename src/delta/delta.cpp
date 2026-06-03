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
delta::delta(const std::string& srcdiff_filename,
                       const METHOD_TYPE& method,
                       srcml_archive* archive,
                       const std::optional<std::string>& unit_filename,
                       const client_options::view_options_t& view_options,
                       const std::optional<std::string>& summary_type_str)
  : archive(archive), output(std::make_shared<srcdiff::output_stream>(archive,
                             srcdiff_filename,
                             method,
                             view_options,
                             summary_type_str)),
    unit_filename(unit_filename) {}

// destructor
delta::~delta() {
  output->close();
}

void delta::write_delta() {
  output->write_unit();
}

}
