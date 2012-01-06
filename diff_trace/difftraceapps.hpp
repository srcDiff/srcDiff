#ifndef INCLUDED_DIFFTRACEAPPS_HPP
#define INCLUDED_DIFFTRACEAPPS_HPP

// options
const char * const HELP_FLAG = "help";
const char HELP_FLAG_SHORT = 'h';

const char * const VERSION_FLAG = "version";
const char VERSION_FLAG_SHORT = 'v';

const char * const WHITESPACE_FLAG = "whitespace";
const char WHITESPACE_FLAG_SHORT = 'w';

const long OPTION_WHITESPACE = 1 << 0;

const char * const SRCML_RELATIVE_FLAG = "srcdiff-relative";
const char SRCML_RELATIVE_FLAG_SHORT = 'd';

const long OPTION_SRCML_RELATIVE = 1 << 1;

// error flags
#define SUCCESS 0
#define INVALID_OPTION 1

#endif
