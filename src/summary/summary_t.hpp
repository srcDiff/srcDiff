#ifndef INCLUDED_SUMMARY_T_HPP
#define INCLUDED_SUMMARY_T_HPP

#include <srcdiff_type.hpp>
#include <summary_output_stream.hpp>
#include <summary_manip.hpp>

class summary_t {

    public:

        enum summary_name_t { IDENTIFIER, REPLACE, MOVE, INTERCHANGE, JUMP, CONDITIONAL, EXPR_STMT, EXPR_STMT_CALLS, CALL_SEQUENCE, DECL_STMT, EXCEPTION, LABEL, NONE };

    protected:

        bool output_verb;
        summary_name_t type;
        srcdiff_type operation;
        size_t count;

        static std::string get_article(const std::string & type_name) { 

            const char letter = type_name[0];

            if(letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u')
                return "an";
            else
                return "a";
        }

    public:

        summary_t(summary_name_t type, srcdiff_type operation, bool output_verb = false)
            : type(type), operation(operation), count(1), output_verb(output_verb) {}

        virtual bool compare(const summary_t & summary) const {

            return false;

        }

        virtual bool operator==(const summary_t & summary) const {

            if(type == summary.type && operation == summary.operation) return compare(summary);

            return false;

        }

        virtual summary_t & operator+=(const summary_t & summary) {

            count += summary.count;

            return *this;

        }

        virtual summary_output_stream & output(summary_output_stream & out) const {

            return out;

        }

};

#endif
