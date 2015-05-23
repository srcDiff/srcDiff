#ifndef INCLUDED_EXPR_STMT_SUMMARY_T_HPP
#define INCLUDED_EXPR_STMT_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class expr_stmt_summary_t : public summary_t {

    protected:

        std::string statement_type;

    public:

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

        virtual summary_output_stream & output(summary_output_stream & out, size_t count) const {

            out.begin_line();

            if(count == 1)
                out << get_article(statement_type) << ' ' << manip::bold() << statement_type << manip::normal() << " was ";
            else
                out << std::to_string(count) << ' ' << manip::bold() << statement_type << 's' << manip::normal() << " were ";

            out << (operation == SRCDIFF_DELETE ?  "deleted" : (operation == SRCDIFF_INSERT ? "inserted" : "modified"));

            out << '\n';

            return out;

        }

};

#endif
