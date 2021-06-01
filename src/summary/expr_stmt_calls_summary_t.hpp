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
 
        expr_stmt_calls_summary_t(std::string statement_type,
        						  size_t number_deleted, size_t number_inserted, size_t number_renamed,
        						  size_t number_argument_list_modified, size_t number_arguments_deleted, size_t number_arguments_inserted, size_t number_arguments_modified)
            : expr_stmt_summary_t(EXPR_STMT_CALLS, SRCDIFF_COMMON, statement_type),
              number_deleted(number_deleted), number_inserted(number_inserted), number_renamed(number_renamed),
              number_argument_list_modified(number_argument_list_modified),
              number_arguments_deleted(number_arguments_deleted), number_arguments_inserted(number_arguments_inserted), number_arguments_modified(number_arguments_modified) {}

        virtual bool compare(const summary_t & summary) const {

            const expr_stmt_calls_summary_t & expr_stmt_calls_summary = dynamic_cast<const expr_stmt_calls_summary_t &>(summary);
            return bool(number_deleted) == bool(expr_stmt_calls_summary.number_deleted) && bool(number_inserted) == bool(expr_stmt_calls_summary.number_inserted)
                && bool(number_renamed) == bool(expr_stmt_calls_summary.number_renamed) && bool(number_argument_list_modified) == bool(expr_stmt_calls_summary.number_argument_list_modified)
                && bool(number_arguments_deleted)  == bool(expr_stmt_calls_summary.number_arguments_deleted)
                && bool(number_arguments_inserted) == bool(expr_stmt_calls_summary.number_arguments_inserted)
                && bool(number_arguments_modified) == bool(expr_stmt_calls_summary.number_arguments_modified);

        }

        virtual summary_t & operator+=(const summary_t & summary) {

            const expr_stmt_calls_summary_t & expr_stmt_calls_summary = dynamic_cast<const expr_stmt_calls_summary_t &>(summary);

            count += expr_stmt_calls_summary.count;

            number_deleted                 += expr_stmt_calls_summary.number_deleted;
            number_inserted                += expr_stmt_calls_summary.number_inserted;
            number_renamed                 += expr_stmt_calls_summary.number_renamed;
            number_argument_list_modified  += expr_stmt_calls_summary.number_argument_list_modified;

            number_arguments_deleted  += expr_stmt_calls_summary.number_arguments_deleted;
            number_arguments_inserted += expr_stmt_calls_summary.number_arguments_inserted;
            number_arguments_modified += expr_stmt_calls_summary.number_arguments_modified;

            return *this;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            out.begin_line();

            if(number_deleted != 0) {

                if(number_deleted == 1) {
                    out << "a " << manip::bold() << "call" << manip::normal() << ' ';
                    if(output_verb) out << "was ";
                    out << "deleted";
                } else {
                    out << std::to_string(number_deleted) << ' ' << manip::bold() << "calls" << manip::normal() << ' ';
                    if(output_verb) out << "were ";
                    out << "deleted";
                }

            } else if(number_inserted != 0) {

                if(number_inserted == 1) {
                    out << "a " << manip::bold() << "call" << manip::normal() << ' ';
                    if(output_verb) out << "was ";
                    out << "inserted";
                } else {
                    out << std::to_string(number_inserted) << ' ' << manip::bold() << "calls" << manip::normal() << ' ';
                    if(output_verb) out << "were ";
                    out << "inserted";
                }

            } else if(number_renamed != 0) {

                if(number_renamed == 1) {
                    out << "a " << manip::bold() << "call" << manip::normal() << ' ';
                    if(output_verb) out << "was ";
                    out << "renamed";
                } else {
                    out << std::to_string(number_renamed) << ' ' << manip::bold() << "calls" << manip::normal() << ' ';
                    if(output_verb) out << "were ";
                    out << "renamed";
                }

            } else if(number_argument_list_modified != 0) {

                size_t number_arguments_total = number_arguments_deleted + number_arguments_inserted + number_arguments_modified;

                if(number_argument_list_modified == 1) {

                    size_t number_argument_operations = 0;
                    if(number_arguments_deleted != 0)  ++number_argument_operations;
                    if(number_arguments_inserted != 0) ++number_argument_operations;
                    if(number_arguments_modified != 0) ++number_argument_operations;

                    if(number_argument_operations == 1) {

                        if(number_arguments_total == 1) {
                            out << "an " << manip::bold() << "argument" << manip::normal() << ' ';
                            if(output_verb) out << "was ";
                        } else {
                            out << std::to_string(number_arguments_total) << ' ' << manip::bold() << "arguments" << manip::normal() << ' ';
                            if(output_verb) out << "were ";
                        }

                        if(number_arguments_deleted > 0) {
                            out << "deleted";
                        } else if(number_arguments_inserted > 0) {
                            out << "inserted";
                        } else {
                            out << "modified";
                        }

                    } else {

                        out << "an " << manip::bold() << "argument list" << manip::normal() << ' ';
                        if(output_verb) out << "was ";
                        out << "modified";

                    }

                } else {

                    out << std::to_string(number_argument_list_modified) << ' ' << manip::bold() << "argument lists" << manip::normal() << ' ';
                    if(output_verb) out << "were ";
                    out << "modified";

                }

            }

            out.end_line();

            return out;

        }

};

#endif
