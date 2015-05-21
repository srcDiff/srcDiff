#ifndef INCLUDED_INTERCHANGE_SUMMARY_T_HPP
#define INCLUDED_INTERCHANGE_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <versioned_string.hpp>

class interchange_summary_t : public summary_t {

    private:

        versioned_string statement_type;

    public:

        interchange_summary_t(versioned_string statement_type)
            : summary_t(INTERCHANGE, SRCDIFF_COMMON), statement_type(statement_type) {}

};

#endif
