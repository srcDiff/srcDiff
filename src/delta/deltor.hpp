// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file deltor.hpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_DELTOR_HPP
#define INCLUDED_DELTOR_HPP

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

class deltor {

private:

  std::shared_ptr<output_stream> output;
  const std::optional<std::string>& unit_filename;

public:

  // constructor
  deltor(const METHOD_TYPE& method, const std::optional<std::string>& unit_filename);

  // destructor
  ~deltor();

  srcml_unit* create(srcml_archive* archive, input_stream_manager& manager,
                     const std::string& language,
                     const std::optional<std::string>& unit_filename  = std::optional<std::string>(),
                     const std::optional<std::string>& unit_version   = std::optional<std::string>());

};

}

#endif
