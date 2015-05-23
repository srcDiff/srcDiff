#ifndef INCLUDED_EXPR_STMT_SUMMARY_T_HPP
#define INCLUDED_EXPR_STMT_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class expr_stmt_summary_t : public summary_t {

    protected:

        std::string statement_type;

    public:

        friend class text_summary;

        expr_stmt_summary_t(srcdiff_type operation,
                       		std::string statement_type)
            : summary_t(EXPR_STMT, operation), statement_type(statement_type) {}

        expr_stmt_summary_t(summary_name_t type, srcdiff_type operation,
                       		std::string statement_type)
            : summary_t(type, operation), statement_type(statement_type) {}

        virtual bool compare(const summary_t & summary) const {

            const expr_stmt_summary_t & expr_stmt_summary = dynamic_cast<const expr_stmt_summary_t &>(summary);
            return operation == expr_stmt_summary.operation && statement_type == expr_stmt_summary.statement_type;

        }

};

#endif
