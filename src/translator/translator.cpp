// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file translator.cpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <translator.hpp>

namespace srcdiff {

// constructor
translator::translator(const std::string & srcdiff_filename,
                                       const OPTION_TYPE & flags,
                                       const METHOD_TYPE & method,
                                       srcml_archive * archive,
                                       const std::optional<std::string> & unit_filename,
                                       const client::options::view_options_t & view_options,
                                       const std::optional<std::string> & summary_type_str)
  : archive(archive), flags(flags), output(std::make_shared<srcdiff::output_stream>(archive,
                                           srcdiff_filename,
                                           flags,
                                           method,
                                           view_options,
                                           summary_type_str)),
    unit_filename(unit_filename) {}


// destructor
translator::~translator() {
  output->close();
}

void translator::write_translation() {
  output->write_unit();
}

}
