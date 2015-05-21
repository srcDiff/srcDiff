#ifndef INCLUDED_IDENTIFIER_SUMMARY_T_HPP
#define INCLUDED_IDENTIFIER_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <versioned_string.hpp>

class identifier_summary_t : public summary_t {

    private:

        versioned_string name;
        bool is_complex;

    public:

        identifier_summary_t(versioned_string name, bool is_complex)
            : summary_t(IDENTIFIER, SRCDIFF_COMMON), name(name), is_complex(is_complex) {}

};

#endif
