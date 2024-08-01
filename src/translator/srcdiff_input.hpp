// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_input.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_INPUT_HPP
#define INCLUDED_SRCDIFF_INPUT_HPP

#include <srcml_nodes.hpp>
#include <srcdiff_options.hpp>
#include <srcml_converter.hpp>

#include <srcml.h>

#include <vector>
#include <memory>

#include <optional>

template<class T>
class srcdiff_input {

protected:

    srcml_archive * archive;
    const std::optional<std::string> input_path;
    const char * language_string;
    const OPTION_TYPE & options;
    const T & input;

private:

public:

    srcdiff_input(srcml_archive * archive, const std::optional<std::string> & input_path, const char * language_string, const OPTION_TYPE & options, const T & input);
    ~srcdiff_input();

    void operator()(int stream_source, srcml_nodes & nodes, int & is_input, const srcml_converter::srcml_burst_config & burst_config) const;

    virtual srcml_nodes input_nodes(int stream_source, const srcml_converter::srcml_burst_config & burst_config) const;

};

#include <srcdiff_input.tcc>

#endif
