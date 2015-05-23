#include <text_summary.hpp>

#include <conditional_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <call_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <if_profile_t.hpp>
#include <call_profile_t.hpp>
#include <expr_profile_t.hpp>
#include <ternary_profile_t.hpp>
#include <identifier_profile_t.hpp>
#include <identifier_diff.hpp>
#include <summary_manip.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <set>
#include <typeinfo>
#include <cstring>

text_summary::text_summary(const change_entity_map<parameter_profile_t> & parameters, const change_entity_map<call_profile_t> & member_initializations)
    : parameters(parameters), member_initializations(member_initializations) {}

summary_output_stream & text_summary::parameter(summary_output_stream & out, size_t number_parameters_deleted,
                                       size_t number_parameters_inserted, size_t number_parameters_modified) const {

    if(number_parameters_deleted > 0) {

        out.begin_line();

        if(number_parameters_deleted == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was deleted";

        else
            out << std::to_string(number_parameters_deleted) << ' ' << manip::bold() << "parameters" << manip::normal() << " were deleted\n";

    }

    if(number_parameters_inserted > 0) {

        out.begin_line();

        if(number_parameters_inserted == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was inserted\n";
        else
            out << std::to_string(number_parameters_inserted) << ' ' << manip::bold() << "parameters" << manip::normal() << " were inserted\n";

    }

    if(number_parameters_modified > 0) {

        out.begin_line();

        if(number_parameters_modified == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was modified\n";
        else
            out << std::to_string(number_parameters_modified) << ' ' << manip::bold() << "parameters" << manip::normal() << " were modified\n";

    }

    return out;

}

summary_output_stream & text_summary::member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                                   size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const {

    if(number_member_initializations_deleted > 0) {

        out.begin_line();

        if(number_member_initializations_deleted == 1)
            out << "a " << manip::bold() << "member initilization" << manip::normal() << " was deleted\n";
        else
            out << std::to_string(number_member_initializations_deleted) << ' ' << manip::bold() << "member initilizations" << manip::normal() << " were deleted\n";

    }

    if(number_member_initializations_inserted > 0) {

        out.begin_line();

        if(number_member_initializations_inserted == 1)
            out << "a " << manip::bold() << "member initilization" << manip::normal() << " was inserted\n";
        else
            out << std::to_string(number_member_initializations_inserted) << ' ' << manip::bold() << "member initilizations" << manip::normal() << " were inserted\n";

    }

    if(number_member_initializations_modified > 0) {

        out.begin_line();

        if(number_member_initializations_modified == 1)
            out << "a " << manip::bold() << "member initilization" << manip::normal() << " was modified\n";
        else
            out << std::to_string(number_member_initializations_modified) << ' ' << manip::bold() << "member initilizations" << manip::normal() << " were modified\n";

    }

    return out;

}

summary_output_stream & text_summary::body(summary_output_stream & out, const std::vector<summary_t *> summaries) {

    for(std::vector<summary_t *>::size_type pos = 0; pos < summaries.size(); ++pos) {

        if((pos + 1) < summaries.size() && (*summaries[pos]) == (*summaries[pos + 1])) {

            (*summaries[pos + 1]) += (*summaries[pos]);
            continue;

        }

        summaries[pos]->output(out);    

    }

    return out;

}
