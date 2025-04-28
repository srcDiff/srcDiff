// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file common_stream.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_COMMON_STREAM_HPP
#define INCLUDED_COMMON_STREAM_HPP

#include <output_stream.hpp>

namespace srcdiff {

class common_stream : public output_stream {

protected:

    unsigned int end_original;
    unsigned int end_modified;

private:

    static const std::optional<std::string> whitespace;
    virtual void markup_common();

public:

    static void output_common(std::shared_ptr<output_stream> out, int end_original, int end_modified) {
        common_stream common(*out, end_original, end_modified);
        common.output();
    }

    common_stream(const output_stream& out, unsigned int end_original, unsigned int end_modified);

    virtual void output();

};

}

#endif
