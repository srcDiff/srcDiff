// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file input_stream.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_INPUT_STREAM_HPP
#define INCLUDED_INPUT_STREAM_HPP

#include <srcml_nodes.hpp>
#include <srcml_converter.hpp>
#include <client_options.hpp>

#include <srcml.h>

#include <vector>
#include <memory>

#include <optional>

namespace srcdiff {

template<class T>
class input_stream {

protected:

    srcml_archive * archive;
    const std::optional<std::string> input_path;
    const char * language_string;
    const OPTION_TYPE & options;
    const T & input;

private:

public:

    input_stream(srcml_archive * archive, const std::optional<std::string> & input_path, const char * language_string, const OPTION_TYPE & options, const T & input);
    ~input_stream();

    void operator()(int stream_source, srcml_nodes & nodes, int & is_input) const;

    virtual srcml_nodes input_nodes(int stream_source) const;

};

#include <input_stream.tcc>

}

#endif
