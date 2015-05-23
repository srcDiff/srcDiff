#ifndef INCLUDED_CONDITIONAL_SUMMARY_T_HPP
#define INCLUDED_CONDITIONAL_SUMMARY_T_HPP

#include <summary_t.hpp>

#include <string>

class conditional_summary_t : public summary_t {

    private:

        std::string statement_type;
        bool condition_modified;

    public:

        friend class text_summary;

        conditional_summary_t(srcdiff_type operation,
                       		  std::string statement_type, bool condition_modified)
            : summary_t(CONDITIONAL, operation), statement_type(statement_type), condition_modified(condition_modified) {}

        virtual bool compare(const summary_t & summary) const {

            const conditional_summary_t & conditional_summary = dynamic_cast<const conditional_summary_t &>(summary);
            return statement_type == conditional_summary.statement_type && condition_modified == conditional_summary.condition_modified;

        }

};

#endif
