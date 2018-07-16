#ifndef INCLUDED_MOVE_SUMMARY_T_HPP
#define INCLUDED_MOVE_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class move_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        move_summary_t(std::string statement_type)
            : summary_t(MOVE, SRCDIFF_COMMON), statement_type(statement_type) {}

        virtual bool compare(const summary_t & summary) const {

            const move_summary_t & move_summary = dynamic_cast<const move_summary_t &>(summary);
            return statement_type == move_summary.statement_type;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(count == 1)
                out << get_article(statement_type) << ' ' << manip::bold() << statement_type << manip::normal() << " was moved";
            else
                out << std::to_string(count) << ' ' << manip::bold() << statement_type << 's' << manip::normal() << " were moved";

            out.end_line();

            return out;

        }

};

#endif
