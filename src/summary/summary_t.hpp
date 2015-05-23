#ifndef INCLUDED_SUMMARY_T_HPP
#define INCLUDED_SUMMARY_T_HPP

#include <srcdiff_type.hpp>
#include <srcdiff_macros.hpp>
#include <type_query.hpp>

class summary_t {

    public:

        enum summary_name_t { IDENTIFIER, REPLACEMENT, MOVE, INTERCHANGE, JUMP, CONDITIONAL, EXPR_STMT, EXPR_STMT_CALLS, CALL_SEQUENCE, DECL_STMT, NONE };

    protected:

        summary_name_t type;
        srcdiff_type operation;

    public:

        friend class text_summary;

        summary_t(summary_name_t type, srcdiff_type operation)
            : type(type), operation(operation) {}

        virtual bool compare(const summary_t & summary) const {

            return false;

        }

        virtual bool operator==(const summary_t & summary) const {

            if(type == summary.type && operation == summary.operation) return compare(summary);

            return false;

        }

};

#endif
