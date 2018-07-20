#ifndef INCLUDED_MACRO_SUMMARY_T_HPP
#define INCLUDED_MACRO_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class macro_summary_t : public summary_t {

    protected:

    public:

        macro_summary_t(srcdiff_type operation)
            : summary_t(EXPR_STMT, operation) {}

        macro_summary_t(summary_name_t type, srcdiff_type operation)
            : summary_t(type, operation) {}

        virtual bool compare(const summary_t & summary) const {

            const macro_summary_t & macro_summary = dynamic_cast<const macro_summary_t &>(summary);
            return operation == macro_summary.operation;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(count == 1) {
                out << get_article("macro") << ' ' << manip::bold() << "macro" << manip::normal();
                if(output_verb) out << " was";
            } else {
                out << std::to_string(count) << ' ' << manip::bold() << "macro" << 's' << manip::normal();
                if(output_verb) out << " were";
            }

            out << (operation == SRCDIFF_DELETE ?  " deleted" : (operation == SRCDIFF_INSERT ? " inserted" : " modified"));

            out.end_line();

            return out;

        }

};

#endif
