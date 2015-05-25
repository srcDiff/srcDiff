#ifndef INCLUDED_CALL_SEQUENCE_SUMMARY_T_HPP
#define INCLUDED_CALL_SEQUENCE_SUMMARY_T_HPP

#include <expr_stmt_summary_t.hpp>

#include <string>

class call_sequence_summary_t : public expr_stmt_summary_t {

    private:

    	bool name_change;
    	bool variable_reference_change;

    public:

        call_sequence_summary_t(std::string statement_type,
        						bool name_change, bool variable_reference_change)
            : expr_stmt_summary_t(CALL_SEQUENCE, SRCDIFF_COMMON, statement_type), name_change(name_change), variable_reference_change(variable_reference_change) {}


        virtual bool compare(const summary_t & summary) const {

        	const call_sequence_summary_t & call_sequence_summary = dynamic_cast<const call_sequence_summary_t &>(summary);
            return name_change == call_sequence_summary.name_change && variable_reference_change == call_sequence_summary.variable_reference_change;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(name_change) {

                out << "a " << manip::bold() << "name" << manip::normal() << " change occurred to ";

                if(count == 1)
                    out << "a " << manip::bold() << "call" << manip::normal();
                else
                    out << std::to_string(count) << ' ' << manip::bold() << "calls" << manip::normal();

            } else if(variable_reference_change) {

                if(count == 1)
                    out << "a ";
                else
                    out << std::to_string(count) << ' ';

                out << manip::bold() << "variable reference" << manip::normal();

                if(count == 1)
                    out << " change ";
                else
                    out << " changes ";

                out << "occurred";

            }

            out << '\n';

            return out;
        }

};

#endif
