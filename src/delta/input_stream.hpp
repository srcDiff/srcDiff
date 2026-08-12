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

class abstract_input_stream {
public:
    virtual ~abstract_input_stream() {}
    virtual void operator()(srcML::converter& converter, srcML::nodes& nodes) const = 0;
private:
};

class input_stream_base : public abstract_input_stream {
public:
    input_stream_base(const std::optional<std::string>& path) : path(path) {}

    virtual void operator()(srcML::converter& converter, srcML::nodes& nodes) const override {
        return;
    }

    const std::optional<std::string> get_path() { return path; }
protected:
    const std::optional<std::string> path;
};

template<class T>
class input_stream : public input_stream_base {
public:

    input_stream(const T& input, const std::optional<std::string>& path,
                 srcml_archive* archive, const char* language);
    ~input_stream();

    virtual void operator()(srcML::converter& converter, srcML::nodes& nodes) const override;

protected:
    const T& input;

    srcml_archive* archive;
    const char* language;
};

#include <input_stream.tcc>

}

#endif
