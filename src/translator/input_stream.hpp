// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_stream.hpp
 *
 * @copyright Copyright (C) 2014-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_INPUT_STREAM_HPP
#define INCLUDED_INPUT_STREAM_HPP

#include <srcml_nodes.hpp>
#include <srcml_converter.hpp>
#include <client_options.hpp>

#include <srcml.h>

#include <optional>

namespace srcdiff {

class input_stream_base {
public:
    virtual void operator()(srcml_nodes& nodes, srcml_archive* archive, const OPTION_TYPE& options) const = 0;
    virtual srcml_nodes input_nodes(srcml_archive* archive, const OPTION_TYPE& options) const = 0;
};

template<class T>
class input_stream : public input_stream_base {
public:

    input_stream(const T& input, const std::optional<std::string>& input_path, const char* language_string);
    ~input_stream();

    virtual void operator()(srcml_nodes& nodes, srcml_archive* archive, const OPTION_TYPE& options) const;
    virtual srcml_nodes input_nodes(srcml_archive* archive, const OPTION_TYPE& options) const;

protected:
    const T& input;
    const std::optional<std::string> input_path;
    const char* language_string;
};

#include <input_stream.tcc>

}

#endif
