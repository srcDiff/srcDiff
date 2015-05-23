#ifndef INCLUDED_EXPR_STMT_CALLS_SUMMARY_T_HPP
#define INCLUDED_EXPR_STMT_CALLS_SUMMARY_T_HPP

#include <expr_stmt_summary_t.hpp>

#include <string>

class expr_stmt_calls_summary_t : public expr_stmt_summary_t {

    private:

    	size_t number_deleted;
    	size_t number_inserted;
    	size_t number_renamed;
    	size_t number_argument_list_modified;
    	size_t number_arguments_deleted;
    	size_t number_arguments_inserted;
    	size_t number_arguments_modified;

    public:

        friend class text_summary;
 
        expr_stmt_calls_summary_t(std::string statement_type,
        						  size_t number_deleted, size_t number_inserted, size_t number_renamed,
        						  size_t number_argument_list_modified, size_t number_arguments_deleted, size_t number_arguments_inserted, size_t number_arguments_modified)
            : expr_stmt_summary_t(EXPR_STMT_CALLS, SRCDIFF_COMMON, statement_type),
              number_deleted(number_deleted), number_inserted(number_inserted), number_renamed(number_renamed),
              number_argument_list_modified(number_argument_list_modified),
              number_arguments_deleted(number_arguments_deleted), number_arguments_inserted(number_arguments_inserted), number_arguments_modified(number_arguments_modified) {}

        virtual bool compare(const summary_t & summary) const {

            const expr_stmt_calls_summary_t & expr_stmt_calls_summary = dynamic_cast<const expr_stmt_calls_summary_t &>(summary);
            return false;
            // return number_deleted == expr_stmt_calls_summary.number_deleted && number_inserted == expr_stmt_calls_summary.number_inserted
            //     && number_renamed == expr_stmt_calls_summary.number_renamed && number_argument_list_modified == expr_stmt_calls_summary.number_argument_list_modified
            //     && number_arguments_deleted == expr_stmt_calls_summary.number_arguments_deleted
            //     && number_arguments_inserted == expr_stmt_calls_summary.number_arguments_inserted
            //     && number_arguments_modified == expr_stmt_calls_summary.number_arguments_modified;

        }

        virtual summary_output_stream & output(summary_output_stream & out, size_t count) const {

            out.begin_line();

            if(number_deleted != 0) {

                if(number_deleted == 1)
                    out << "a " << manip::bold() << "call" << manip::normal() << " was deleted";
                else
                    out << std::to_string(number_deleted) << ' ' << manip::bold() << "calls" << manip::normal() << " were deleted";

            } else if(number_inserted != 0) {

                if(number_inserted == 1)
                    out << "a " << manip::bold() << "call" << manip::normal() << " was inserted";
                else
                    out << std::to_string(number_inserted) << ' ' << manip::bold() << "calls" << manip::normal() << " were inserted";

            } else if(number_renamed != 0) {

                if(number_renamed == 1)
                    out << "a " << manip::bold() << "call" << manip::normal() << " was renamed";
                else
                    out << std::to_string(number_renamed) << ' ' << manip::bold() << "calls" << manip::normal() << " were renamed";

            } else if(number_argument_list_modified != 0) {

                size_t number_arguments_total = number_arguments_deleted + number_arguments_inserted + number_arguments_modified;

                if(number_argument_list_modified == 1) {

                    if(number_arguments_total == 1) {

                        out << "an " << manip::bold() << "argument" << manip::normal() << " was ";

                        if(number_arguments_deleted == 1)
                            out << "deleted";
                        else if(number_arguments_inserted == 1)
                            out << "inserted";
                        else
                            out << "modified";

                    } else {

                        //out << std::to_string(number_arguments_total) << ' ' << manip::bold() << "arguments" << manip::normal() << " were modified";
                        out << "an " << manip::bold() << "argument list" << manip::normal() << " was modified";

                    }

                } else {

                    out << manip::bold() << "argument lists" << manip::normal() << " were modified";

                }

            }

            out << '\n';

            return out;

        }

};

#endif
