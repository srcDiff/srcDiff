#include <text_summary.hpp>

#include <iterator>

text_summary::text_summary() {}

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

summary_output_stream & text_summary::body(summary_output_stream & out, std::list<summary_t *> & summaries) const {

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
