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

#include <nodes.hpp>
#include <converter.hpp>
#include <client_options.hpp>

#include <srcml.h>

#include <optional>
#include <filesystem>

namespace srcdiff {

class input_stream_base {
public:
    virtual ~input_stream_base() {}
    virtual void operator()(srcML::converter& converter, srcML::nodes& nodes) const = 0;
    virtual srcML::nodes input_nodes(srcML::converter& converter) const = 0;
private:
};

template<class T>
class input_stream : public input_stream_base {
public:

    input_stream(const T& input, const std::optional<std::string>& path,
                 srcml_archive* archive,
                 const char* language);
    ~input_stream();

    virtual void operator()(srcML::converter& converter, srcML::nodes& nodes) const;
    virtual srcML::nodes input_nodes(srcML::converter& converter) const;

protected:
    const T& input;
    const std::optional<std::string> path;

    srcml_archive* archive;
    const char* language;
};

#include <input_stream.tcc>

}

#endif
