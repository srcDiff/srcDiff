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


        friend class text_summary;

};

#endif
