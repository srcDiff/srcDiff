// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file change_stream.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_CHANGE_STREAM_HPP
#define INCLUDED_CHANGE_STREAM_HPP

#include <output_stream.hpp>

namespace srcdiff {

class change_stream : public srcdiff::output_stream {

protected:
 
    std::size_t end_original;
    std::size_t end_modified;

private:

public:

    static void output_change(std::shared_ptr<srcdiff::output_stream> out, std::size_t end_original, std::size_t end_modified) {
     change_stream change(*out, end_original, end_modified);
     change.output();
    }

    change_stream(const srcdiff::output_stream & out, std::size_t end_original, std::size_t end_modified);

    virtual void output_whitespace_all();
    virtual void output_whitespace_prefix();
    virtual void output();

};

}

#endif
