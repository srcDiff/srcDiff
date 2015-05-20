#ifndef INCLUDED_IDENTIFIER_SUMMARY_T_HPP
#define INCLUDED_IDENTIFIER_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <versioned_string.hpp>

class identifier_summary_t : public summary_t {

    private:

        versioned_string name;
        bool is_complex;

    public:

        identifier_summary_t(summary_name_t type, srcdiff_type operation,
                             versioned_string name, bool is_complex)
            : summary_t(type, operation), name(name), is_complex(is_complex) {}

};

#endif
