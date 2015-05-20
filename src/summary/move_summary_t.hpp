#ifndef INCLUDED_MOVE_SUMMARY_T_HPP
#define INCLUDED_MOVE_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class move_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        move_summary_t(summary_name_t type, namespace_uri uri, srcdiff_type operation,
                       std::string statement_type)
            : summary_t(type, uri, operation), statement_type(statement_type) {}

};

#endif
