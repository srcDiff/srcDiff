#ifndef INCLUDED_CONDITIONAL_SUMMARY_T_HPP
#define INCLUDED_CONDITIONAL_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class conditional_summary_t : public summary_t {

    private:

        std::string statement_type;
        bool condition_modified;

    public:

        conditional_summary_t(srcdiff_type operation,
                       		  std::string statement_type, bool condition_modified)
            : summary_t(CONDITIONAL, operation), statement_type(statement_type), condition_modified(condition_modified) {}

        virtual bool compare(const summary_t & summary) const {

            const conditional_summary_t & conditional_summary = dynamic_cast<const conditional_summary_t &>(summary);
            return statement_type == conditional_summary.statement_type && condition_modified == conditional_summary.condition_modified;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            if(condition_modified) {

                 out.begin_line();


                 out << "the condition of ";

                 if(count == 1)
                     out << get_article(statement_type) << ' ' << manip::bold() << statement_type << manip::normal() << " was ";
                 else
                     out << std::to_string(count) << ' ' << manip::bold() << statement_type << 's' << manip::normal() << " were ";

                 out << "altered\n";

            }

            if(operation != SRCDIFF_COMMON) {

                out.begin_line();

                if(count == 1)
                    out << get_article(statement_type) << ' ' << manip::bold() << statement_type << manip::normal() << " was ";
                else
                    out << std::to_string(count) << ' ' << manip::bold() << statement_type << 's' << manip::normal() << " were ";

                out << (operation == SRCDIFF_DELETE ? "deleted" : "inserted");

                out << '\n';

            }

            return out;

        }

};

#endif
