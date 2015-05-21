#ifndef INCLUDED_EXPR_STMT_SUMMARY_T_HPP
#define INCLUDED_EXPR_STMT_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class expr_stmt_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        expr_stmt_summary_t(summary_name_t type, srcdiff_type operation,
                       		std::string statement_type)
            : summary_t(type, operation), statement_type(statement_type) {}

};

#endif
