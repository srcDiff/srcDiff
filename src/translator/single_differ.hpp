// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file single_differ.hpp
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SINGLE_DIFFER_HPP
#define INCLUDED_SINGLE_DIFFER_HPP

#include <construct.hpp>
#include <output_stream.hpp>

namespace srcdiff {

class single_differ {

protected:

    std::shared_ptr<output_stream> out;

    std::shared_ptr<const construct> original_construct;
    std::shared_ptr<const construct> modified_construct;

private:

    void output_recursive_same();
    void output_recursive_interchangeable();

public:

    single_differ(std::shared_ptr<output_stream> out, std::shared_ptr<const construct> original_construct,  std::shared_ptr<const construct> modified_construct);

    virtual void output();

};

}

#endif
