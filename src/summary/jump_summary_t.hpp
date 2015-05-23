#ifndef INCLUDED_JUMP_SUMMARY_T_HPP
#define INCLUDED_JUMP_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class jump_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        friend class text_summary;

        jump_summary_t(srcdiff_type operation,
                       std::string statement_type)
            : summary_t(JUMP, operation), statement_type(statement_type) {}


        virtual bool compare(const summary_t & summary) const {

        	const jump_summary_t & jump_summary = dynamic_cast<const jump_summary_t &>(summary);
            return statement_type == jump_summary.statement_type;

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
