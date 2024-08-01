// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcdiff_single.hpp
 *
 * @copyright Copyright (C) 2012-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCDIFF_SINGLE_HPP
#define INCLUDED_SRCDIFF_SINGLE_HPP

#include <construct.hpp>
#include <srcdiff_output.hpp>

class srcdiff_single {

protected:

    std::shared_ptr<srcdiff_output> out;

    std::shared_ptr<const construct> original_construct;
    std::shared_ptr<const construct> modified_construct;

private:

    void output_recursive_same();
    void output_recursive_interchangeable();

public:

    srcdiff_single(std::shared_ptr<srcdiff_output> out, std::shared_ptr<const construct> original_construct,  std::shared_ptr<const construct> modified_construct);

    virtual void output();

};

#endif
