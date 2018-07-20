#ifndef INCLUDED_DECL_STMT_SUMMARY_T_HPP
#define INCLUDED_DECL_STMT_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class decl_stmt_summary_t : public summary_t {

    private:

        bool specifiers_operation;
    	bool type_modified;
    	bool name_modified;
    	bool init_modified;

    public:

        decl_stmt_summary_t(srcdiff_type operation,
				        	bool specifiers_operation, bool type_modified,	bool name_modified,	bool init_modified)
            : summary_t(DECL_STMT, operation),
              specifiers_operation(specifiers_operation), type_modified(type_modified), name_modified(name_modified), init_modified(init_modified) {}

        virtual bool compare(const summary_t & summary) const {

            const decl_stmt_summary_t & decl_stmt_summary = dynamic_cast<const decl_stmt_summary_t &>(summary);
            return type_modified == decl_stmt_summary.type_modified && name_modified == decl_stmt_summary.name_modified && init_modified == decl_stmt_summary.init_modified;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(count == 1)
                out << "a " << manip::bold() << "declaration" << manip::normal();
            else
                out << std::to_string(count) << ' ' << manip::bold() << "declarations" << manip::normal();

            size_t number_parts_report = (specifiers_operation ? 1 : 0) + (type_modified ? 1 : 0) + (name_modified ? 1 : 0) + (init_modified ? 1 : 0);

            if(operation == SRCDIFF_COMMON && number_parts_report == 1) {

                out << "'s ";

                if(specifiers_operation) {

                    out << "specifiers ";
                    if(output_verb) out << "were ";
                    out << "modified";

                } else if(type_modified) {

                    if(count == 1) {
                        out << "type ";
                        if(output_verb) out << "was ";
                    } else {
                        out << "types ";
                        if(output_verb) out << "were ";
                    }

                    out << "modified";

                } else if(name_modified) {

                    if(count == 1) {
                        out << "name ";
                        if(output_verb) out << "was ";
                    } else {
                        out << "names ";
                        if(output_verb) out << "were ";
                    }

                    out << "changed";

                } else {

                    if(count == 1) {
                        out << "initialization ";
                        if(output_verb) out << "was ";
                    } else {
                        out << "initialiations ";
                        if(output_verb) out << "were ";
                    }

                    out  << "modified";

                }

            } else {

                if(output_verb) {
                    if(count == 1) {
                        out << " was";
                    } else {
                        out << " were";
                    }
                }
                out << (operation == SRCDIFF_DELETE ?  " deleted" : (operation == SRCDIFF_INSERT ? " inserted" : " modified"));

            }

            out.end_line();

            return out;

        }
        
};

#endif
