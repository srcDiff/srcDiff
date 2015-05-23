#ifndef INCLUDED_JUMP_SUMMARY_T_HPP
#define INCLUDED_JUMP_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class jump_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        friend class text_summary;

        jump_summary_t(srcdiff_type operation,
                       std::string statement_type)
            : summary_t(JUMP, operation), statement_type(statement_type) {}


        virtual bool compare(const summary_t & summary) const {

        	const jump_summary_t & jump_summary = dynamic_cast<const jump_summary_t &>(summary);
            return statement_type == jump_summary.statement_type;

        }

};

#endif
