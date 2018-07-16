#include <text_summary.hpp>

#include <iterator>

text_summary::text_summary() {}

summary_output_stream & text_summary::specifier(summary_output_stream & out, const std::multimap<srcdiff_type, std::string> & specifiers) const {

    for(std::map<srcdiff_type, std::string>::const_iterator citr = specifiers.lower_bound(SRCDIFF_DELETE); citr != specifiers.upper_bound(SRCDIFF_DELETE); ++citr) {
        out.begin_line() << manip::bold() << citr->second << manip::normal() << " specifier was deleted";
        out.end_line();
    }
    for(std::map<srcdiff_type, std::string>::const_iterator citr = specifiers.lower_bound(SRCDIFF_INSERT); citr != specifiers.upper_bound(SRCDIFF_INSERT); ++citr) {
        out.begin_line() << manip::bold() << citr->second << manip::normal() << " specifier was inserted";
        out.end_line();              
    }
    for(std::map<srcdiff_type, std::string>::const_iterator citr = specifiers.lower_bound(SRCDIFF_COMMON); citr != specifiers.upper_bound(SRCDIFF_COMMON); ++citr) {
        out.begin_line() << manip::bold() << citr->second << manip::normal() << " specifier was modified";
        out.end_line();
    }

    return out;
}


summary_output_stream & text_summary::parameter(summary_output_stream & out, const std::vector<std::shared_ptr<parameter_profile_t>> & parameters) const {

    size_t number_parameters_deleted = 0, number_parameters_inserted = 0, number_parameters_modified = 0, number_parameters_replaced = 0;
    for(std::vector<std::shared_ptr<parameter_profile_t>>::size_type pos = 0; pos < parameters.size(); ++pos) {

        if(parameters[pos]->is_replace && parameters[pos]->operation == SRCDIFF_DELETE
            && (pos == 0 || !parameters[pos - 1]->is_replace || parameters[pos - 1]->operation != SRCDIFF_DELETE)) {

            if((pos + 1) < parameters.size() && parameters[pos + 1]->operation == SRCDIFF_INSERT
                && ((pos + 2) == parameters.size() || !parameters[pos + 2]->is_replace || parameters[pos + 2]->operation != SRCDIFF_INSERT)) {

                ++number_parameters_replaced;
                ++pos;
                continue;

            }

        }

        if(parameters[pos]->operation == SRCDIFF_DELETE)
            ++number_parameters_deleted;
        else if(parameters[pos]->operation == SRCDIFF_INSERT)
            ++number_parameters_inserted;
        else if(parameters[pos]->operation == SRCDIFF_COMMON)
            ++number_parameters_modified;

    }

    if(number_parameters_replaced > 0) {

        out.begin_line();

        if(number_parameters_replaced == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was replaced\n";
        else
            out << std::to_string(number_parameters_replaced) << ' ' << manip::bold() << "parameters" << manip::normal() << " were replaced\n";

    }

    if(number_parameters_deleted > 0) {

        out.begin_line();

        if(number_parameters_deleted == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was deleted\n";

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

summary_output_stream & text_summary::function_body(summary_output_stream & out, std::list<summary_t *> & summaries) const {

    for(std::list<summary_t *>::iterator itr = summaries.begin(); itr != summaries.end(); ++itr) {

        for(std::list<summary_t *>::iterator next_itr = std::next(itr, 1); next_itr != summaries.end();) {

            if((**itr) == (**next_itr)) {

                (**itr) += (**next_itr);
                delete *next_itr;
                summaries.erase(next_itr++);
                
            } else {

                ++next_itr;

            }

        }

    }

    for(std::list<summary_t *>::iterator itr = summaries.begin(); itr != summaries.end(); ++itr) {

        // std::list<summary_t *>::iterator next_itr = std::next(itr, 1);
        // if(next_itr != summaries.end() && (**itr) == (**next_itr)) {

        //     (**next_itr) += (**itr);
        //     continue;

        // }

        (*itr)->output(out);    

    }

    return out;

}
