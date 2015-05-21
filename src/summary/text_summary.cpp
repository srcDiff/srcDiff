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

/** @todo check asserts */

const char * const BOLD_TEXT = "\x1b[1m";
const char * const NORMAL_TEXT = "\x1b[0m";
std::string bold(std::string text) {

    return BOLD_TEXT + text + NORMAL_TEXT;

}

std::string text_summary::get_article(const std::string & type_name) const { 

    const char letter = type_name[0];

    if(letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u')
        return "an";
    else
        return "a";
}

summary_output_stream & text_summary::summary_dispatch(summary_output_stream & out, const summary_t & summary) {

    switch(summary.type) {

        case summary_t::IDENTIFIER:
            identifier(out, dynamic_cast<const identifier_summary_t &>(summary));
            break;

        case summary_t::REPLACEMENT:
            replacement(out, dynamic_cast<const replacement_summary_t &>(summary));
            break;

        case summary_t::MOVE:
            move(out, dynamic_cast<const move_summary_t &>(summary));
            break;

        case summary_t::INTERCHANGE:
            interchange(out, dynamic_cast<const interchange_summary_t &>(summary));
            break;            

        case summary_t::JUMP:
            jump(out, dynamic_cast<const jump_summary_t &>(summary));
            break;            

        case summary_t::CONDITIONAL:
            conditional(out, dynamic_cast<const conditional_summary_t &>(summary));
            break;            

        case summary_t::EXPR_STMT:
            expr_stmt(out, dynamic_cast<const expr_stmt_summary_t &>(summary));
            break;            

        case summary_t::EXPR_STMT_CALLS:
            expr_stmt_calls(out, dynamic_cast<const expr_stmt_calls_summary_t &>(summary));
            break;    

        case summary_t::CALL_SEQUENCE:
            call_sequence(out, dynamic_cast<const call_sequence_summary_t &>(summary));
            break;    

        case summary_t::DECL_STMT:
            decl_stmt(out, dynamic_cast<const decl_stmt_summary_t &>(summary));
            break;

        default:
            break;

    }

    return out;

}

summary_output_stream & text_summary::identifier(summary_output_stream & out, const identifier_summary_t & identifier) const {

    out.begin_line();

    if(identifier.is_complex) {

        out << "name change from '";
        out << identifier.name.original();
        out << "' to '";
        out << identifier.name.modified();
        out << '\'';


    } else {

        out << '\'';
        out << identifier.name.original();
        out << "' was renamed to '";
        out << identifier.name.modified();
        out << '\'';

    }

    out << '\n';

    return out;

}

summary_output_stream & text_summary::replacement(summary_output_stream & out, const replacement_summary_t & summary) const {

    out.begin_line();

    if(summary.number_original == 0 || summary.number_modified == 0) {

        if(summary.number_original == 1) {

            out << get_article(summary.original_type) << ' ' << manip::bold() << summary.original_type << manip::normal();
            out << " was commented out";

        } else {

            out << get_article(summary.modified_type) << ' ' << manip::bold() << summary.modified_type << 's' << manip::normal();
            out << " was uncommented";

        }

        out << '\n';

        return out;
    }

    if(summary.number_original == 1)
        out << get_article(summary.original_type) << ' ' << manip::bold() << summary.original_type << manip::normal();
    else
        out << std::to_string(summary.number_original) << ' ' << manip::bold() << summary.original_type << 's' << manip::normal();

    if(summary.number_comments_original == 1)
        out << " and a " << manip::bold() << "comment" << manip::normal();
    else if(summary.number_comments_original > 1)
        out << "and " << std::to_string(summary.number_comments_original) << manip::bold() << "comments" << manip::normal();


    if((summary.number_original + summary.number_comments_original) == 1)
        out << " was";
    else
        out << " were";

    out << " replaced with ";

    if(summary.number_modified == 1)
        out << get_article(summary.modified_type) << ' ' << manip::bold() << summary.modified_type << manip::normal();
    else
        out << std::to_string(summary.number_modified) << ' ' << manip::bold() << summary.modified_type << 's' << manip::normal();

    if(summary.number_comments_modified == 1)
        out << " and a " << manip::bold() << "comment" << manip::normal();
    else if(summary.number_comments_modified > 1)
        out << "and " << std::to_string(summary.number_comments_modified) << manip::bold() << "comments" << manip::normal();

    out << '\n';

    return out;

}

summary_output_stream & text_summary::move(summary_output_stream & out, const move_summary_t & summary) const {

    out.begin_line();

    out << get_article(summary.statement_type) << ' ' << manip::bold() << summary.statement_type << manip::normal() << " was moved";

    out << '\n';

    return out;

}

summary_output_stream & text_summary::interchange(summary_output_stream & out, const interchange_summary_t & summary) const {

    out.begin_line();

    out << get_article(summary.statement_type.original()) << ' ' << manip::bold() << summary.statement_type.original() << manip::normal()
        << " was converted to "
        << get_article(summary.statement_type.modified()) << ' ' << manip::bold() << summary.statement_type.modified() << manip::normal();

    out << '\n';

    return out;

}

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

summary_output_stream & text_summary::jump(summary_output_stream & out, const jump_summary_t & summary) const {

    out.begin_line();

    out << get_article(summary.statement_type) << ' '
        << manip::bold() << summary.statement_type << manip::normal();

    out << " was ";

    out << (summary.operation == SRCDIFF_DELETE ?  "deleted" : (summary.operation == SRCDIFF_INSERT ? "inserted" : "modified"));

    out << '\n';

    return out;

}

/** @todo if multiple of same change like test case where connect deleted 4 times.  May want to summarize in one line. */
summary_output_stream & text_summary::conditional(summary_output_stream & out, const conditional_summary_t & summary) const {

    if(summary.condition_modified) {

         out.begin_line() << "the condition of "
                          << get_article(summary.statement_type) << ' '
                          << manip::bold() << summary.statement_type << manip::normal()
                          << " was altered\n";

    }

    if(summary.operation != SRCDIFF_COMMON) {

        out.begin_line();

        out << get_article(summary.statement_type) << ' '
            << manip::bold() << summary.statement_type << manip::normal();

        out << " was ";
        out << (summary.operation == SRCDIFF_DELETE ? "deleted" : "inserted");

        out << '\n';

    }

    return out;

}

summary_output_stream & text_summary::expr_stmt_calls(summary_output_stream & out, const expr_stmt_calls_summary_t & summary) const {

    out.begin_line();

    if(summary.number_deleted != 0) {

        if(summary.number_deleted == 1)
            out << "a " << manip::bold() << "call" << manip::normal() << " was deleted";
        else
            out << std::to_string(summary.number_deleted) << ' ' << manip::bold() << "calls" << manip::normal() << " were deleted";

    } else if(summary.number_inserted != 0) {

        if(summary.number_inserted == 1)
            out << "a " << manip::bold() << "call" << manip::normal() << " was inserted";
        else
            out << std::to_string(summary.number_inserted) << ' ' << manip::bold() << "calls" << manip::normal() << " were inserted";

    } else if(summary.number_renamed != 0) {

        if(summary.number_renamed == 1)
            out << "a " << manip::bold() << "call" << manip::normal() << " was renamed";
        else
            out << std::to_string(summary.number_renamed) << ' ' << manip::bold() << "calls" << manip::normal() << " were renamed";

    } else if(summary.number_argument_list_modified != 0) {

        size_t number_arguments_total = summary.number_arguments_deleted + summary.number_arguments_inserted + summary.number_arguments_modified;

        if(summary.number_argument_list_modified == 1) {

            if(number_arguments_total == 1) {

                out << "an " << manip::bold() << "argument" << manip::normal() << " was ";

                if(summary.number_arguments_deleted == 1)
                    out << "deleted";
                else if(summary.number_arguments_inserted == 1)
                    out << "inserted";
                else
                    out << "modified";

            } else {

                //out << std::to_string(number_arguments_total) << ' ' << manip::bold() << "arguments" << manip::normal() << " were modified";
                out << "an " << manip::bold() << "argument list" << manip::normal() << " was modified";

            }

        } else {

            out << manip::bold() << "argument lists" << manip::normal() << " were modified";

        }

    }

    out << '\n';

    return out;

}

summary_output_stream & text_summary::call_sequence(summary_output_stream & out, const call_sequence_summary_t & summary) const {

    out.begin_line();

    if(summary.name_change) {

        out << "a " << manip::bold() << "name" << manip::normal() << " change occurred to a " << manip::bold() << "call" << manip::normal();

    } else if(summary.variable_reference_change) {

        out << "a " << manip::bold() << "variable reference" << manip::normal() << " change occurred";

    }

    out << '\n';

    return out;

}

summary_output_stream & text_summary::expr_stmt(summary_output_stream & out, const expr_stmt_summary_t & summary) const {

    out.begin_line();

    out << get_article(summary.statement_type) << ' ' << manip::bold() << summary.statement_type << manip::normal() << " was ";

    out << (summary.operation == SRCDIFF_DELETE ?  "deleted" : (summary.operation == SRCDIFF_INSERT ? "inserted" : "modified"));

    out << '\n';

    return out;

}

/** @todo for decl_stmt and jump need to not report if only a known rename identifier occurs.  Also, report a rename if it occurred */
/** @todo report type rename and name rename.  Report as type and name change probably. */
summary_output_stream & text_summary::decl_stmt(summary_output_stream & out, const decl_stmt_summary_t & summary) const {

    out.begin_line();

    out << "a " << manip::bold() << "declaration" << manip::normal();

    size_t number_parts_report = (summary.type_modified ? 1 : 0) + (summary.name_modified ? 1 : 0) + (summary.init_modified ? 1 : 0);

    if(number_parts_report == 1) {

        if(summary.type_modified)
            out << " type was changed";

        if(summary.name_modified)
            out << " name was changed";
        
        if(summary.init_modified)
            out << " initialization was modified";

    } else {

        out << " was ";

        out << (summary.operation == SRCDIFF_DELETE ?  "deleted" : (summary.operation == SRCDIFF_INSERT ? "inserted" : "modified"));

    }

    out << '\n';

    return out;

}



summary_output_stream & text_summary::body(summary_output_stream & out, const std::vector<summary_t *> summaries) {

    for(const summary_t * summary : summaries)
        summary_dispatch(out, *summary);    

    return out;

}
