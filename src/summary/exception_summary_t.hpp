#ifndef INCLUDED_EXCEPTION_SUMMARY_T_HPP
#define INCLUDED_EXCEPTION_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class exception_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        exception_summary_t(srcdiff_type operation,
                       std::string statement_type)
            : summary_t(EXCEPTION, operation), statement_type(statement_type) {}


        virtual bool compare(const summary_t & summary) const {

        	const exception_summary_t & exception_summary = dynamic_cast<const exception_summary_t &>(summary);
            return statement_type == exception_summary.statement_type;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(count == 1)
                out << get_article(statement_type) << ' ' << manip::bold() << statement_type << manip::normal() << " was ";
            else
                out << std::to_string(count) << ' ' << manip::bold() << statement_type << 's' << manip::normal() << " were ";

            out << (operation == SRCDIFF_DELETE ?  "deleted" : (operation == SRCDIFF_INSERT ? "inserted" : "modified"));

            out.end_line();

            return out;

        }

};

#endif
