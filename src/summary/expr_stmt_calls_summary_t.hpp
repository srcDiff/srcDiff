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

};

#endif
