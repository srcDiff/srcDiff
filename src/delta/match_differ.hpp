// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file match_differ.hpp
 *
 * @copyright Copyright (C) 2014-2025 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_MATCH_DIFFER_HPP
#define INCLUDED_MATCH_DIFFER_HPP

#include <construct.hpp>
#include <output_stream.hpp>

namespace srcdiff {

class match_differ {

protected:

    std::shared_ptr<output_stream> out;

    std::shared_ptr<const construct> original_construct;
    std::shared_ptr<const construct> modified_construct;

public:

    match_differ(std::shared_ptr<output_stream> out, std::shared_ptr<const construct> original_construct,  std::shared_ptr<const construct> modified_construct);
    virtual void output();

};

}

#endif
