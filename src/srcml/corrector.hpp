// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file corrector.hpp
 *
 * @copyright Copyright (C) 2026-2026 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SRCML_CONVERTER_HPP
#define INCLUDED_SRCML_CONVERTER_HPP

#include <nodes.hpp>

namespace srcML {

class corrector {
public:
    corrector(srcML::nodes& nodes);

    void correct();

private:
    srcML::nodes& nodes;

};

}

#endif