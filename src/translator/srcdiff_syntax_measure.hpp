/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_SRCDIFF_SYNTAX_MEASURE_HPP
#define INCLUDED_SRCDIFF_SYNTAX_MEASURE_HPP

#include <srcdiff_measure.hpp>

#include <cassert>

class srcdiff_syntax_measure : public srcdiff_measure {

protected:

private:

public:

	srcdiff_syntax_measure(const construct & set_original, const construct & set_modified);

	virtual void compute();

};





#endif
