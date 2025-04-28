// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file syntax_measurer.hpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SYNTAX_MEASURE_HPP
#define INCLUDED_SYNTAX_MEASURE_HPP

#include <measurer.hpp>

#include <cassert>
namespace srcdiff {

class syntax_measurer : public measurer {

protected:

private:

public:

    syntax_measurer(const construct & set_original, const construct & set_modified);

    virtual void compute();

};

}

#endif
