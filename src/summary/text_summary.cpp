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
            //move(out, dynamic_cast<const identifier_summary_t &>(summary));
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

    if(summary.number_original == 1)
        out << get_article(summary.original_type) << manip::bold() << summary.original_type << manip::normal();
    else
        out << std::to_string(summary.number_original) << manip::bold() << summary.original_type << manip::normal();

    out << " were replaced with ";

    if(summary.number_original == 1)
        out << get_article(summary.original_type) << manip::bold() << summary.original_type << manip::normal();
    else
        out << std::to_string(summary.number_original) << manip::bold() << summary.original_type << manip::normal();

    out << '\n';

    return out;

}

summary_output_stream & text_summary::interchange(summary_output_stream & out, const interchange_summary_t & summary) const {

    out.begin_line();

    out << get_article(summary.statement_type.original()) << manip::bold() << summary.statement_type.original() << manip::normal()
        << " was converted to "
        << get_article(summary.statement_type.modified()) << manip::bold() << summary.statement_type.modified() << manip::normal();

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

// summary_output_stream & text_summary::common_expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) const {

//     assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

//     const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

//     const std::shared_ptr<profile_t> & parent_profile = profile->parent;
//     std::map<identifier_diff, size_t> diff_set;
//     std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
//                         output_identifiers.begin(), output_identifiers.end(),
//                         std::inserter(diff_set, diff_set.begin()));

//     std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists;
//     std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;
//     size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;
//     bool identifier_rename_only = true;
//     std::set<std::reference_wrapper<const versioned_string>> identifier_renames; 
//     expr_statistics(profile->child_profiles[0], diff_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,
//                     deleted_other, inserted_other, modified_other,
//                     number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
//                     identifier_rename_only, identifier_renames);

//     if(deleted_calls.size() == 0 && inserted_calls.size() == 0 && modified_calls.size() == 0
//     && deleted_other.size() == 0 && inserted_other.size() == 0 && modified_other.size() == 0
//     && identifier_renames.size() == 0) return out;

//     if(expr_stmt_profile->call())
//         return call_sequence(out, profile, renamed_calls.size(), number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
//                              modified_argument_lists.size(), identifier_rename_only, identifier_renames);

//     out.begin_line();

//     size_t number_change_types = 0;
//     if(deleted_calls.size() != 0)           ++number_change_types;
//     if(inserted_calls.size() != 0)          ++number_change_types;
//     if(renamed_calls.size() != 0)           ++number_change_types;
//     if(modified_argument_lists.size() != 0) ++number_change_types;
//     if(deleted_other.size() != 0)           ++number_change_types;
//     if(inserted_other.size() != 0)          ++number_change_types;
//     if(modified_other.size() != 0)          ++number_change_types;

//     if(identifier_rename_only && identifier_renames.size() == 1) {

//         out << '\'' << identifier_renames.begin()->get().original() << "' was renamed to '" << identifier_renames.begin()->get().modified() << '\'';

//     } else if(number_change_types == 1) {

//         if(deleted_calls.size() != 0) {

//             if(deleted_calls.size() == 1)
//                 out << "a " << manip::bold() << "call" << manip::normal() << " was deleted";
//             else
//                 out << std::to_string(deleted_calls.size()) << ' ' << manip::bold() << "calls" << manip::normal() << " were deleted";

//         } else if(inserted_calls.size() != 0) {

//             if(inserted_calls.size() == 1)
//                 out << "a " << manip::bold() << "call" << manip::normal() << " was inserted";
//             else
//                 out << std::to_string(inserted_calls.size()) << ' ' << manip::bold() << "calls" << manip::normal() << " were inserted";

//         } else if(renamed_calls.size() != 0) {

//             if(renamed_calls.size() == 1)
//                 out << "a " << manip::bold() << "call" << manip::normal() << " was renamed";
//             else
//                 out << std::to_string(renamed_calls.size()) << ' ' << manip::bold() << "calls" << manip::normal() << " were renamed";

//         } else if(modified_argument_lists.size() != 0) {

//             if(modified_argument_lists.size() == 1) {

//                 if(modified_argument_lists[0]->child_profiles.size() == 1)
//                     out << "an " << manip::bold() << "argument" << manip::normal() << " was modified";
//                 else
//                     out << std::to_string(modified_argument_lists[0]->child_profiles.size()) << manip::bold() << "arguments" << manip::normal() << " were modified";

//             } else {

//                 out << manip::bold() << "argument lists" << manip::normal() << " were modified";

//             }

//         } else {

//            out << get_profile_string(profile) << " was modified";

//         }

//     } else {

//        out << get_profile_string(profile) << " was modified";

//     }

//     out << '\n';

//     return out;

// }

// static bool operator<(const std::__1::reference_wrapper<const versioned_string> & ref_one, const std::__1::reference_wrapper<const versioned_string> & ref_two) {

//     return ref_one.get() < ref_two.get();

// }

// summary_output_stream & text_summary::call_sequence(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t number_rename,
//                                                     size_t number_arguments_deleted, size_t number_arguments_inserted, size_t number_arguments_modified,
//                                                     size_t number_argument_lists_modified,
//                                                     bool identifier_rename_only, const std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

//     assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

//     const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

//     std::vector<std::shared_ptr<call_profile_t>>::size_type calls_sequence_length = expr_stmt_profile->get_call_profiles().size();

//     size_t number_arguments_total = number_arguments_deleted + number_arguments_inserted + number_arguments_modified;
//     bool is_variable_reference_change = number_argument_lists_modified == 0;

//     if(is_variable_reference_change) {

//         for(std::vector<std::shared_ptr<call_profile_t>>::size_type pos = 0; pos < calls_sequence_length; ++pos) {

//             const std::shared_ptr<call_profile_t> & call_profile = expr_stmt_profile->get_call_profiles()[pos];

//             if((call_profile->operation == SRCDIFF_COMMON && call_profile->argument_list_modified)
//                 || (call_profile->operation != SRCDIFF_COMMON && pos != (calls_sequence_length - 1))) {

//                 is_variable_reference_change = false;
//                 break;

//             }

//        }

//     }

//     out.begin_line();

//     if(number_rename == 1 && identifier_renames.size() == 0 && number_argument_lists_modified == 0) {

//         out << "a " << manip::bold() << "name" << manip::normal() << " change occurred to a " << manip::bold() << "call" << manip::normal();

//     } else if(is_variable_reference_change) {

//         out << "a " << manip::bold() << "variable reference" << manip::normal() << " change occurred";

//     } else if(identifier_rename_only && identifier_renames.size() == 1) {

//         out << '\'' << identifier_renames.begin()->get().original() << "' was renamed to '" << identifier_renames.begin()->get().modified() << '\'';

//     } else if(number_argument_lists_modified == 1 && number_rename == 0) {

//         if(number_arguments_total == 1) {

//             out << "an " << manip::bold() << "argument" << manip::normal() << " was ";

//             if(number_arguments_deleted == 1)
//                 out << "deleted";
//             else if(number_arguments_inserted == 1)
//                 out << "inserted";
//             else
//                 out << "modified";

//         } else {

//            out << "an " << manip::bold() << "argument list" << manip::normal() << " was modified";

//         }

//     } else {

//         out << get_profile_string(profile) << " was modified";

//     }

//     out << '\n';

//     return out;

// }

summary_output_stream & text_summary::expr_stmt(summary_output_stream & out, const expr_stmt_summary_t & summary) const {

    // assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

    // const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    // if((expr_stmt_profile->assignment() && expr_stmt_profile->operation != SRCDIFF_COMMON) || expr_stmt_profile->is_delete() || profile->child_profiles.empty()) {

    //     out.begin_line() << get_profile_string(profile) << " was ";

    //     out << (profile->operation == SRCDIFF_DELETE ?  "deleted" : (profile->operation == SRCDIFF_INSERT ? "inserted" : "modified"));

    //     if(abstract_level != HIGH && (profile->parent == id || !parent_output)) {

    //         if(profile->operation == SRCDIFF_DELETE)      out << " from ";
    //         else if(profile->operation == SRCDIFF_INSERT) out << " to ";
    //         else                                          out << " within ";

    //         if(profile->parent == id)
    //             out << "the function";
    //         else
    //             out << "a nested " << get_type_string(profile->parent);

    //     }

    //     out << '\n';

    //     return out;

    // }

    // if(profile->operation == SRCDIFF_COMMON) {

    //     common_expr_stmt(out, profile);

    // }

    return out;

}

/** @todo for decl_stmt and jump need to not report if only a known rename identifier occurs.  Also, report a rename if it occurred */
/** @todo report type rename and name rename.  Report as type and name change probably. */
summary_output_stream & text_summary::decl_stmt(summary_output_stream & out, const decl_stmt_summary_t & summary) const {

    // assert(typeid(*profile.get()) == typeid(decl_stmt_profile_t));

    // const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);

    // const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    // std::map<identifier_diff, size_t> identifier_set;
    // std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
    //                     output_identifiers.begin(), output_identifiers.end(),
    //                     std::inserter(identifier_set, identifier_set.begin()));

    // size_t number_parts_report = 0;
    // bool identifier_rename_only = true;
    // std::set<std::reference_wrapper<const versioned_string>> identifier_renames;
    // if(decl_stmt_profile->operation == SRCDIFF_COMMON) {

    //     if(!decl_stmt_profile->type.is_common()) {

    //         identifier_diff ident_diff(decl_stmt_profile->type);
    //         ident_diff.trim(true);

    //         if(identifier_set.count(ident_diff))
    //             ++number_parts_report;

    //     }

    //     if(!decl_stmt_profile->name.is_common()) {

    //         identifier_diff ident_diff(decl_stmt_profile->name);
    //         ident_diff.trim(true);

    //         if(identifier_set.count(ident_diff))
    //             ++number_parts_report;

    //     }

    //     if(!decl_stmt_profile->init.is_common()) {

    //         std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists;
    //         std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;
    //         size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;
    //         std::set<std::reference_wrapper<const versioned_string>> identifier_renames; 
    //         expr_statistics(decl_stmt_profile->child_profiles.back(), identifier_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,
    //                         deleted_other, inserted_other, modified_other,
    //                         number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
    //                         identifier_rename_only, identifier_renames);

    //         /** @todo need to probably output if single identifier change */
    //         if(deleted_calls.size() != 0 || inserted_calls.size() != 0 || modified_calls.size() != 0
    //         || deleted_other.size() != 0 || inserted_other.size() != 0 || modified_other.size() != 0
    //         || identifier_renames.size() != 0)
    //             ++number_parts_report;

    //     }

    //     if(number_parts_report == 0) return out;

    // }

    // out.begin_line() << get_profile_string(decl_stmt_profile);

    // if(number_parts_report == 1) {

    //     if(!decl_stmt_profile->type.is_common()) {

    //         out << " type was changed";

    //     }

    //    if(!decl_stmt_profile->name.is_common()) {

    //         out << " name was changed";

    //     } else {

    //      if(identifier_rename_only && identifier_renames.size() == 1)
    //         out << '\'' << identifier_renames.begin()->get().original() << "' was renamed to '" << identifier_renames.begin()->get().modified() << '\'';
    //     else
    //         out << " initialization was modified";


    //     }

    // } else {

    //     out << " was ";

    //     out << (profile->operation == SRCDIFF_DELETE ?  "deleted" : (profile->operation == SRCDIFF_INSERT ? "inserted" : "modified"));

    //     if(abstract_level != HIGH && (profile->parent == id || !parent_output)) {

    //         if(profile->operation == SRCDIFF_DELETE)      out << " from ";
    //         else if(profile->operation == SRCDIFF_INSERT) out << " to ";
    //         else                                          out << " within ";

    //         if(profile->parent == id)
    //             out << "the function";
    //         else
    //             out << "a nested " << get_type_string(profile->parent);

    //     }

    // }

    // out << '\n';

    return out;

}

/** @todo if multiple of same change like test case where connect deleted 4 times.  May want to summarize in one line. */
summary_output_stream & text_summary::conditional(summary_output_stream & out, const conditional_summary_t & summary) const {

    // assert(is_condition_type(profile->type_name));

    // const bool has_common = profile->common_profiles.size() > 0;

    // const std::shared_ptr<conditional_profile_t> & conditional_profile = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile);

    // const bool condition_modified = conditional_profile->is_condition_modified();
    // const bool body_modified = conditional_profile->is_body_modified();

    // boost::optional<srcdiff_type> else_operation;
    // if(profile->type_name == "if") else_operation = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->else_operation();
    // const bool else_modified = bool(else_operation) && *else_operation == SRCDIFF_COMMON;

    // boost::optional<srcdiff_type> elseif_operation;
    // if(profile->type_name == "if") elseif_operation = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->elseif_operation();;
    // const bool elseif_modified = bool(elseif_operation) && *elseif_operation == SRCDIFF_COMMON;

    // const versioned_string & condition = conditional_profile->get_condition();

    // if(!condition_modified && !body_modified && bool(else_operation)
    //     && (profile->operation == SRCDIFF_COMMON || profile->child_profiles.back()->common_profiles.size() > 0))
    //     return else_clause(out, profile->child_profiles[0], parent_output);

    // const std::shared_ptr<profile_t> & summary_profile = profile->type_name == "elseif" && profile->child_profiles.size() == 1
    //     && profile->child_profiles[0]->type_name == "if" ? profile->child_profiles[0] : profile;

    // const bool output_conditional = summary_profile->operation != SRCDIFF_COMMON || condition_modified || number_child_changes(summary_profile->child_profiles) > 1;

    // size_t statement_count = summary_profile->operation == SRCDIFF_DELETE ? summary_profile->statement_count_original : summary_profile->statement_count_modified;
    // if(profile->type_name == "elseif") --statement_count;
    // const size_t common_statements = summary_profile->common_statements;

    // if(condition_modified) {

    //      out.begin_line() << "the condition of " << get_profile_string(profile) << " was altered\n";

    // }

    // if(summary_profile->summary_identifiers.size() > 0) {

    //     identifiers(out, summary_profile->summary_identifiers);

    // }

    // // before children
    // bool is_leaf = true;
    // if(profile->operation != SRCDIFF_COMMON) {

    //     out.begin_line();

    //     out << get_profile_string(profile);
    //     out << " was ";
    //     out << (profile->operation == SRCDIFF_DELETE ? "deleted" : "inserted");

    //     // after children
    //     if(abstract_level != HIGH && (summary_profile->parent == id && summary_profile->operation == SRCDIFF_COMMON)) {

    //         if(summary_profile->operation == SRCDIFF_DELETE)      out << " from ";
    //         else if(summary_profile->operation == SRCDIFF_INSERT) out << " to ";
    //         else                                          out << " within ";

    //         out << "the function";

    //     }

    //     out << '\n';

    // }

    // ++body_depth;

    // for(size_t pos = 0; pos < summary_profile->child_profiles.size(); ++pos) {

    //     const std::shared_ptr<profile_t> & child_profile = summary_profile->child_profiles[pos];

    //     if((child_profile->syntax_count > 0 || child_profile->move_id
    //         || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
    //         && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

    //         statement_dispatch(out, summary_profile, pos, output_conditional);

    //     }

    // }

    // --body_depth;

    return out;

}

summary_output_stream & text_summary::jump(summary_output_stream & out, const jump_summary_t & summary) const {

    // assert(is_jump(profile->type_name));

    // const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    // std::map<identifier_diff, size_t> identifier_set;
    // std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
    //                     output_identifiers.begin(), output_identifiers.end(),
    //                     std::inserter(identifier_set, identifier_set.begin()));

    // if(profile->operation == SRCDIFF_COMMON) {

    //     std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists;
    //     std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;
    //     size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;
    //     bool identifier_rename_only = true;
    //     std::set<std::reference_wrapper<const versioned_string>> identifier_renames;
    //     expr_statistics(profile->child_profiles.back(), identifier_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,
    //                     deleted_other, inserted_other, modified_other,
    //                     number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
    //                     identifier_rename_only, identifier_renames);

    //     /** @todo need to probably output if single identifier change */
    //     if(deleted_calls.size() == 0 && inserted_calls.size() == 0 && modified_calls.size() == 0
    //     && deleted_other.size() == 0 && inserted_other.size() == 0 && modified_other.size() == 0
    //     && identifier_renames.size() == 0)
    //         return out;

    // }

    // out.begin_line() << get_profile_string(profile);

    // out << " was ";

    // out << (profile->operation == SRCDIFF_DELETE ?  "deleted" : (profile->operation == SRCDIFF_INSERT ? "inserted" : "modified"));

    // if(abstract_level != HIGH && (profile->parent == id || !parent_output)) {

    //     if(profile->operation == SRCDIFF_DELETE)      out << " from ";
    //     else if(profile->operation == SRCDIFF_INSERT) out << " to ";
    //     else                                          out << " within ";

    //     if(profile->parent == id)
    //         out << "the function";
    //     else
    //         out << "a nested " << get_type_string(profile->parent);

    // }

    // out << '\n';

    return out;

}

summary_output_stream & text_summary::body(summary_output_stream & out, const std::vector<summary_t> summaries) {

    for(const summary_t & summary : summaries) {



    }
    

    return out;

}
