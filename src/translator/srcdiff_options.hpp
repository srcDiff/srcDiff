#ifndef INCLUDED_SRCDIFF_OPTIONS_HPP
#define INCLUDED_SRCDIFF_OPTIONS_HPP

#include <Options.hpp>

// @srcdiff
const OPTION_TYPE OPTION_VISUALIZE      = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SAME     = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_PURE     = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_CHANGE         = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SRCDIFFONLY    = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_DIFFONLY       = ull(1) << __COUNTER__;

// @srcdiff
const OPTION_TYPE OPTION_SVN_CONTINUOUS = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_SVN            = ull(1) << __COUNTER__;

// @src2srcml
const OPTION_TYPE OPTION_BASH_VIEW      = ull(1) << __COUNTER__;

#endif

