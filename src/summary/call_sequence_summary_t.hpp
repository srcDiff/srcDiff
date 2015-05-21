#ifndef INCLUDED_CALL_SEQUENCE_SUMMARY_T_HPP
#define INCLUDED_CALL_SEQUENCE_SUMMARY_T_HPP

#include <expr_stmt_summary_t.hpp>

#include <string>

class call_sequence_summary_t : public expr_stmt_summary_t {

    private:

    	bool is_name_change;
    	bool is_variable_reference_change;

    public:

        call_sequence_summary_t(summary_name_t type, srcdiff_type operation, std::string statement_type,
        						bool is_name_change, bool is_variable_reference_change)
            : expr_stmt_summary_t(type, operation, statement_type), is_name_change(is_name_change), is_variable_reference_change(is_variable_reference_change) {}

};

#endif
