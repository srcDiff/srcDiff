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

        virtual bool compare(const summary_t & summary) const {

            const interchange_summary_t & interchange_summary = dynamic_cast<const interchange_summary_t &>(summary);
            return statement_type == interchange_summary.statement_type;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(count == 1) {
                out << get_article(statement_type.original()) << ' ' << manip::bold() << statement_type.original() << manip::normal();
                if(output_verb) out << " was";
                out << " converted to "
                    << get_article(statement_type.modified()) << ' ' << manip::bold() << statement_type.modified() << manip::normal();
            } else {
                out << std::to_string(count) << ' ' << manip::bold() << statement_type.original() << 's' << manip::normal();
                if(output_verb) out << " were";
                out << " converted to "
                    << manip::bold() << statement_type.modified() << 's' << manip::normal();
            }

            out.end_line();

            return out;

        }

};

#endif
