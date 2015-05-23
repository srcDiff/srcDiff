#ifndef INCLUDED_MOVE_SUMMARY_T_HPP
#define INCLUDED_MOVE_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class move_summary_t : public summary_t {

    private:

        std::string statement_type;

    public:

        friend class text_summary;

        move_summary_t(std::string statement_type)
            : summary_t(MOVE, SRCDIFF_COMMON), statement_type(statement_type) {}

        virtual bool compare(const summary_t & summary) const {

            const move_summary_t & move_summary = dynamic_cast<const move_summary_t &>(summary);
            return statement_type == move_summary.statement_type;

        }

};

#endif
