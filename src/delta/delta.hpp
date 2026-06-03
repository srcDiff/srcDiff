// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file delta.hpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_TRANSLATOR_HPP
#define INCLUDED_TRANSLATOR_HPP

#include <input_stream_manager.hpp>
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

class delta {

private:

  std::shared_ptr<output_stream> output;
  const std::optional<std::string>& unit_filename;

public:

  // constructor
  delta(srcml_archive * archive, const METHOD_TYPE& method, const std::optional<std::string>& unit_filename);

  // destructor
  ~delta();

  std::string create(input_stream_manager& manager,
                     const std::string& language,
                     const std::optional<std::string>& unit_filename  = std::optional<std::string>(),
                     const std::optional<std::string>& unit_version   = std::optional<std::string>());

  // Think about taking out archive writing and putting having in input
  // separates concerns better
  void write_delta();
};

}

#endif
