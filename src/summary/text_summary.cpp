#include <text_summary.hpp>

#include <conditional_profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <call_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <if_profile_t.hpp>
#include <identifier_profile_t.hpp>
#include <identifier_diff.hpp>
#include <call_profile_t.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <set>

std::string text_summary::get_article(const std::shared_ptr<profile_t> & profile) const { 

    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
    if(is_guard_clause) return "a";

    if(is_expr_stmt(profile->type_name)
        && (reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile)->is_delete()
            || reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile)->call()))
        return "a";

    const char letter = std::string(profile->type_name)[0];

    if(letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u')
        return "an";
    else
        return "a";
}

std::string text_summary::get_type_string(const std::shared_ptr<profile_t> & profile) const {

    if(profile->type_name == "if") {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);
        if(if_profile->is_guard()) return "guard clause";

    }

    if(profile->type_name == "else") return "else-clause";

    if(is_decl_stmt(profile->type_name)) return "declaration statement";

    if(is_expr_stmt(profile->type_name)) {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);
        if(expr_stmt_profile->assignment()) return "assignment statement";
        if(expr_stmt_profile->is_delete())  return "delete statement";
        if(expr_stmt_profile->call())       return "call statement";
        return "expression statement";

    }

    if(is_comment(profile->type_name)) return profile->type_name;

    return profile->type_name + " statement";

}

std::string text_summary::get_profile_string(const std::shared_ptr<profile_t> & profile) const {

    if(profile->type_name == "if") {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);
        if(if_profile->else_clause()) return "an if statement with an else-clause";

    }

    if(is_decl_stmt(profile->type_name)) {

        const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);

        std::string decl_stmt_summary = get_article(decl_stmt_profile) + ' ' + get_type_string(decl_stmt_profile) + " declaring '";
        if(decl_stmt_profile->operation == SRCDIFF_DELETE)
            decl_stmt_summary += decl_stmt_profile->name.original() + "' of type '" + decl_stmt_profile->type.original() + '\'';
        else if(decl_stmt_profile->operation == SRCDIFF_INSERT)
            decl_stmt_summary += decl_stmt_profile->name.modified() + "' of type '" + decl_stmt_profile->type.modified() + '\'';
        else 
            decl_stmt_summary += decl_stmt_profile->name + "' of type '" + decl_stmt_profile->type + '\'';

        return decl_stmt_summary;

    }

    if(is_expr_stmt(profile->type_name)) {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

        if(expr_stmt_profile->assignment()) {

            std::string expr_stmt_summary = "an assignment to '";
            if(expr_stmt_profile->operation == SRCDIFF_DELETE)      expr_stmt_summary += expr_stmt_profile->lhs().original() + '\'';
            else if(expr_stmt_profile->operation == SRCDIFF_INSERT) expr_stmt_summary += expr_stmt_profile->lhs().modified() + '\'';
            else                                                    expr_stmt_summary += std::string(expr_stmt_profile->lhs()) + '\'';

            return expr_stmt_summary;

        }

        if(expr_stmt_profile->call()) {

                std::string expr_stmt_summary;
                if(expr_stmt_profile->get_call_profiles().size() == 1)
                    expr_stmt_summary = get_article(profile) + " call to '";
                else
                    expr_stmt_summary = "a call chain consisting of '";

                size_t count = 0;

                for(const std::shared_ptr<call_profile_t> & call_profile : expr_stmt_profile->get_call_profiles()) {

                    if(expr_stmt_profile->operation == SRCDIFF_DELETE)      expr_stmt_summary += call_profile->name.original() + '\'';
                    else if(expr_stmt_profile->operation == SRCDIFF_INSERT) expr_stmt_summary += call_profile->name.modified() + '\'';
                    else                                                    expr_stmt_summary += std::string(call_profile->name) + '\'';

                    ++count;

                    if(expr_stmt_profile->get_call_profiles().size() == 2 && count == 1)
                        expr_stmt_summary += " and ";
                    else if(expr_stmt_profile->get_call_profiles().size() > 2 && count != expr_stmt_profile->get_call_profiles().size())
                        expr_stmt_summary += ", ";
                    else if(expr_stmt_profile->get_call_profiles().size() > 2 && count == expr_stmt_profile->get_call_profiles().size())
                        expr_stmt_summary += ", and ";

                }

                return expr_stmt_summary;

        }

    }

    if(is_call(profile->type_name)) {

        const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(profile);
        return "a call to '" + call_profile->name.original() + "' was renamed to '" + call_profile->name.modified();

    }

    return get_article(profile) + ' ' + get_type_string(profile);

}

summary_output_stream & text_summary::identifiers(summary_output_stream & out, const std::map<versioned_string, size_t> & identifiers) {

    for(std::pair<versioned_string, size_t> identifier : identifiers) {

        if(identifier.second <= 1) continue;

        out.begin_line() << "the identifier '";

        if(identifier.first.has_original()) out << identifier.first.original() << "' ";

        if(identifier.first.has_original() && identifier.first.has_modified()) out << " was replaced with '";

        if(identifier.first.has_modified()) out << identifier.first.modified() << "' ";

        if(identifier.first.has_original() && !identifier.first.has_modified())
            out << "was removed ";
        else if(!identifier.first.has_original() && identifier.first.has_modified())
            out << "was added ";

        out << "in several places\n";

    }

    for(std::map<versioned_string, size_t>::const_iterator itr = identifiers.begin(); itr != identifiers.end(); ++itr) {

        std::map<versioned_string, size_t>::iterator itersect_itr = output_identifiers.find(itr->first);
        if(itersect_itr == output_identifiers.end())
            output_identifiers.insert(itersect_itr, *itr);
        else
            itersect_itr->second += itr->second;

    }

    return out;

}

summary_output_stream & text_summary::replacement(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t & pos) const {

    const std::shared_ptr<profile_t> & start_profile = profile->child_profiles[pos];

    std::vector<const std::shared_ptr<expr_stmt_profile_t> *>   expr_stmt_deleted,    expr_stmt_inserted;
    std::vector<const std::shared_ptr<decl_stmt_profile_t> *>   decl_stmt_deleted,    decl_stmt_inserted;
    std::vector<const std::shared_ptr<conditional_profile_t> *> conditionals_deleted, conditionals_inserted;
    std::vector<const std::shared_ptr<profile_t> *>             comment_deleted,      comment_inserted;
    for(; pos < profile->child_profiles.size() && profile->child_profiles[pos]->is_replacement; ++pos) {

        const std::shared_ptr<profile_t> & replacement_profile = profile->child_profiles[pos];                    

        if(is_condition_type(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE)
                conditionals_deleted.push_back(reinterpret_cast<const std::shared_ptr<conditional_profile_t> *>(&replacement_profile));
            else
                conditionals_inserted.push_back(reinterpret_cast<const std::shared_ptr<conditional_profile_t> *>(&replacement_profile));

        } else if(is_expr_stmt(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE)
                expr_stmt_deleted.push_back(reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> *>(&replacement_profile));
            else
                expr_stmt_inserted.push_back(reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> *>(&replacement_profile));

        } else if(is_decl_stmt(replacement_profile->type_name)){

            if(replacement_profile->operation == SRCDIFF_DELETE)
                decl_stmt_deleted.push_back(reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> *>(&replacement_profile));
            else
                decl_stmt_inserted.push_back(reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> *>(&replacement_profile));

        } else if(is_comment(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE) comment_deleted.push_back(&replacement_profile);
            else                                                 comment_inserted.push_back(&replacement_profile);

        }

    }

    --pos;

    out.begin_line();

    size_t number_deleted_types  = 0;
    if(expr_stmt_deleted.size() != 0)    ++number_deleted_types;
    if(decl_stmt_deleted.size() != 0)    ++number_deleted_types;
    if(conditionals_deleted.size() != 0) ++number_deleted_types;
    if(comment_deleted.size() != 0)      ++number_deleted_types;

    size_t number_inserted_types = 0;
    if(expr_stmt_inserted.size() != 0)    ++number_inserted_types;
    if(decl_stmt_inserted.size() != 0)    ++number_inserted_types;
    if(conditionals_inserted.size() != 0) ++number_inserted_types;
    if(comment_inserted.size() != 0)      ++number_inserted_types;

    if((((expr_stmt_deleted.size() + decl_stmt_deleted.size() + conditionals_deleted.size()) == 1 
            && (expr_stmt_inserted.size() + decl_stmt_inserted.size() + conditionals_inserted.size()) == 0)
        || ((expr_stmt_inserted.size() + decl_stmt_inserted.size() + conditionals_inserted.size()) == 1
            && (expr_stmt_deleted.size() + decl_stmt_deleted.size() + conditionals_deleted.size()) == 0))
        && (comment_deleted.size() >= 1 || comment_inserted.size() >= 1)) {

        if(expr_stmt_deleted.size())
            out << get_profile_string(*expr_stmt_inserted.back());
        else if(expr_stmt_inserted.size())
            out << get_profile_string(*expr_stmt_inserted.back());
        else if(decl_stmt_deleted.size())
            out << get_profile_string(*decl_stmt_deleted.back());
        else if(decl_stmt_inserted.size())
            out << get_profile_string(*decl_stmt_inserted.back());
        else if(conditionals_deleted.size())
            out << get_profile_string(*conditionals_deleted.back());
        else if(conditionals_inserted.size())
            out << get_profile_string(*conditionals_inserted.back());

        out << " was ";

        if(comment_deleted.size())
            out << "uncommented\n";
        else
            out << "commented out\n";

        return out;

    }

    if((expr_stmt_deleted.size() + decl_stmt_deleted.size() + conditionals_deleted.size() + comment_deleted.size()) == 1) {

        if(expr_stmt_deleted.size())
            out << get_profile_string(*expr_stmt_deleted.back());
        else if(decl_stmt_deleted.size())
            out << get_profile_string(*decl_stmt_deleted.back());
        else if(conditionals_deleted.size())
            out << get_profile_string(*conditionals_deleted.back());
        else
            out << get_profile_string(*comment_deleted.back());

        out << " was";

    } else {

        if(expr_stmt_deleted.size()) {

            if(expr_stmt_deleted.size() == 1)
                out << get_profile_string(*expr_stmt_deleted.back());
            else
                out << "several expression statements";

            if(number_deleted_types == 2)
                out << " and ";
            else if(number_deleted_types > 2)
                out << ", ";

        }

        if(decl_stmt_deleted.size()) {

            if(decl_stmt_deleted.size() == 1)
                out << get_profile_string(*decl_stmt_deleted.back());
            else
                out << "several declaration statements";

            if(expr_stmt_deleted.size() && number_deleted_types == 3)
                out << ", and ";
            else if(expr_stmt_deleted.size() == 0 && number_deleted_types == 2)
                out << " and ";
            else if(number_deleted_types > 2)
                out << ", ";

        }

        if(conditionals_deleted.size()) {

            if(conditionals_deleted.size() == 1)
                out << get_profile_string(*conditionals_deleted.back());
            else
                out << "several conditional statements";

            if(number_deleted_types > 2)
                out << ", and ";
            else if(comment_deleted.size() == 0 && number_deleted_types == 2)
                out << " and ";

        }

        if(comment_deleted.size() == 1)
            out << "a comment";
        else if(comment_deleted.size() > 1)
            out << "several comments";

        out << " were";

    }

    out << " replaced with ";

    if((expr_stmt_inserted.size() + decl_stmt_inserted.size() + conditionals_inserted.size() + comment_inserted.size()) == 1) {
 
        if(expr_stmt_inserted.size())
            out << get_profile_string(*expr_stmt_inserted.back());
        else if(decl_stmt_inserted.size())
            out << get_profile_string(*decl_stmt_inserted.back());
        else if(conditionals_inserted.size())
            out << get_profile_string(*conditionals_inserted.back());
        else
            out << get_profile_string(*comment_inserted.back());

    } else {

        if(expr_stmt_inserted.size()) {

            if(expr_stmt_inserted.size() == 1)
                out << get_profile_string(*expr_stmt_inserted.back());
            else
                out << "several expression statements";

            if(expr_stmt_inserted.size() > 1) out << 's';

            if(number_inserted_types == 2)
                out << " and ";
            else if(number_inserted_types > 2)
                out << ", ";

        }

        if(decl_stmt_inserted.size()) {

            if(decl_stmt_inserted.size() == 1)
                out << get_profile_string(*decl_stmt_inserted.back());
            else
                out << "several declaration statements";

            if(expr_stmt_inserted.size() && number_inserted_types == 3)
                out << ", and ";
            else if(expr_stmt_inserted.size() == 0 && number_inserted_types == 2)
                out << " and ";
            else if(number_inserted_types > 2)
                out << ", ";

        }

        if(conditionals_inserted.size()) {

            if(conditionals_inserted.size() == 1)
                out << get_profile_string(*conditionals_inserted.back());
            else
                out << "several conditional statements";

            if(conditionals_inserted.size() > 1) out << 's';

            if(number_inserted_types > 2)
                out << ", and ";
            else if(comment_inserted.size() == 0 && number_inserted_types == 2)
                out << " and ";

        }

        if(comment_inserted.size() == 1)
            out << "a comment";
        else if(comment_inserted.size() > 1)
            out << "several comments";

    }

    out << '\n';

    return out;

}

bool text_summary::is_body_summary(const std::string & type, bool is_replacement) const {

    return is_condition_type(type) || is_expr_stmt(type) || is_decl_stmt(type) || (is_comment(type) && is_replacement);

}

text_summary::text_summary(const size_t id, const profile_t::profile_list_t & child_profiles, const change_entity_map<parameter_profile_t> & parameters,
             const change_entity_map<call_profile_t> & member_initializations,
             const std::map<versioned_string, size_t> & summary_identifiers)
    : id(id), child_profiles(child_profiles), parameters(parameters), member_initializations(member_initializations),
      summary_identifiers(summary_identifiers) {}

summary_output_stream & text_summary::parameter(summary_output_stream & out, size_t number_parameters_deleted,
                                       size_t number_parameters_inserted, size_t number_parameters_modified) const {

    if(number_parameters_deleted > 0) {

        out.begin_line();

        if(number_parameters_deleted == 1) {

            change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_DELETE);
            out << "the parameter '" << param_iterator->second->name.original() << "' of type '" << param_iterator->second->type.original() << "' was removed\n";

        } else {

            out << "the following parameters were removed:\n";

            out.increment_depth();
            std::for_each(parameters.lower_bound(SRCDIFF_DELETE), parameters.upper_bound(SRCDIFF_DELETE),
                [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                    out.begin_line() << '\'' << parameter.second->name.original() << "' of type '" << parameter.second->type.original() << "'\n";

                });
            out.decrement_depth();

        }

    }

    if(number_parameters_inserted > 0) {

        out.begin_line();

        if(number_parameters_inserted == 1) {

            change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_INSERT);
            out << "the parameter '" << param_iterator->second->name.modified() << "' of type '" << param_iterator->second->type.modified() << "' was added\n";

        } else {

            out << "the following parameters were added:\n";

            out.increment_depth();
            std::for_each(parameters.lower_bound(SRCDIFF_INSERT), parameters.upper_bound(SRCDIFF_INSERT),
                [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                    out.begin_line() << '\'' << parameter.second->name.modified() << "' of type '" << parameter.second->type.modified() << "'\n";

                });
            out.decrement_depth();

        }

    }

    if(number_parameters_modified > 0) {

        out.begin_line();

        if(number_parameters_modified == 1) {

            change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_COMMON);
            out << "the parameter ";
            if(param_iterator->second->name.is_common() && !param_iterator->second->type.is_common())
                out << '\'' << param_iterator->second->name << "' had its type changed from '" 
                    << param_iterator->second->type.original() << "' to '" << param_iterator->second->type.modified() << '\'';
            else if(!param_iterator->second->name.is_common() && param_iterator->second->type.is_common())
                out << "of type '" << param_iterator->second->type << "' had its name changed from '" << param_iterator->second->name.original() << "' to '" << param_iterator->second->name.modified() << '\'';
            else
                out << "name was changed from '" << param_iterator->second->name.original() << "' to '" << param_iterator->second->name.modified() << '\''
                    << " and the type was changed from '" << param_iterator->second->type.original() << "' to '" << param_iterator->second->type.modified() << '\'';;

            out << '\n';

        } else {

            out << "the following parameters were modified:\n";

            out.increment_depth();
            std::for_each(parameters.lower_bound(SRCDIFF_COMMON), parameters.upper_bound(SRCDIFF_COMMON),
                [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                    out.begin_line() << "the parameter ";
                    if(parameter.second->name.is_common() && !parameter.second->type.is_common())
                        out << '\'' << parameter.second->name << "' had its type changed from '" 
                            << parameter.second->type.original() << "' to '" << parameter.second->type.modified() << '\'';
                    else if(!parameter.second->name.is_common() && parameter.second->type.is_common())
                        out << "of type '" << parameter.second->type << "' had its name changed from '" << parameter.second->name.original() << "' to '" << parameter.second->name.modified() << '\'';
                    else
                        out << "name was changed from '" << parameter.second->name.original() << "' to '" << parameter.second->name.modified() << '\''
                            << " and the type was changed from '" << parameter.second->type.original() << "' to '" << parameter.second->type.modified() << '\'';;

                            out << '\n';

                });
            out.decrement_depth();

        }

    }

    return out;

}

summary_output_stream & text_summary::member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                                   size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const {

    if(number_member_initializations_deleted > 0) {

        out.begin_line();

        if(number_member_initializations_deleted == 1) {

            change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_DELETE);
            out << "a member initilization for '" << member_iterator->second->name.original() << "' was removed\n";

        } else {

            out << "the following member initializations were removed:\n";

            out.increment_depth();
            std::for_each(member_initializations.lower_bound(SRCDIFF_DELETE), member_initializations.upper_bound(SRCDIFF_DELETE),
                [&out](const change_entity_map<call_profile_t>::pair & member) {

                    out.begin_line() << '\'' << member.second->name.original() << "'\n";

                });
            out.decrement_depth();

        }

    }

    if(number_member_initializations_inserted > 0) {

        out.begin_line();

        if(number_member_initializations_inserted == 1) {

            change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_INSERT);
            out << "a member initilization for '" << member_iterator->second->name.modified() << "' was added\n";

        } else {

            out << "the following member initializations were added:\n";

            out.increment_depth();
            std::for_each(member_initializations.lower_bound(SRCDIFF_INSERT), member_initializations.upper_bound(SRCDIFF_INSERT),
                [&out](const change_entity_map<call_profile_t>::pair & member) {

                    out.begin_line() << '\'' << member.second->name.modified() << "'\n";

                });
            out.decrement_depth();

        }

    }

    if(number_member_initializations_modified > 0) {

        out.begin_line();

        if(number_member_initializations_modified == 1) {

            change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_COMMON);
            if(member_iterator->second->name.is_common())
                out << "the member initilization for '" << member_iterator->second->name << "' was modified\n";
            else
                out << "the name of a member initilization was changed from '" << member_iterator->second->name.original() << "' to '" << member_iterator->second->name.modified() << "'\n";

        } else {

            out << "the following member initializations were modified:\n";

            out.increment_depth();
            std::for_each(member_initializations.lower_bound(SRCDIFF_COMMON), member_initializations.upper_bound(SRCDIFF_COMMON),
                [&out](const change_entity_map<call_profile_t>::pair & member) {

                    out.begin_line();
                    if(member.second->name.is_common())
                        out << '\'' << member.second->name << "'\n";
                    else
                        out << "the name of a member initilization was changed from '" << member.second->name.original() << "' to '" << member.second->name.modified() << "'\n";

                });
            out.decrement_depth();

        }

    }

    return out;

}

/** @todo call replacement does not seem to be handled  or added/deleted.  This also is more for a lot of changes.  If there is only simpler want to be more specific.*/
void text_summary::expr_stmt_call(const std::shared_ptr<profile_t> & profile, const std::map<versioned_string, size_t> & identifier_set,
                              std::vector<std::shared_ptr<call_profile_t>> & deleted_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & inserted_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & modified_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & renamed_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & modified_argument_lists) const {

    for(const std::shared_ptr<profile_t> & child_profile : profile->child_profiles[0]->child_profiles) {

        const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_profile);

        if(child_profile->type_name.is_common() && is_call(child_profile->type_name)) {

            if(child_profile->operation == SRCDIFF_DELETE) {

                deleted_calls.push_back(call_profile);

            } else if(child_profile->operation == SRCDIFF_INSERT) {

                inserted_calls.push_back(call_profile);

            } else if(child_profile->operation == SRCDIFF_COMMON) {

                bool report_name = !call_profile->name.is_common();
                if(report_name) {

                    identifier_diff ident_diff(call_profile->name);
                    ident_diff.compute_diff();

                    if(!identifier_set.count(ident_diff.get_diff())) report_name = false;

                }

                bool report_argument_list = call_profile->argument_list_modified;
                if(report_argument_list) {

                    size_t number_arguments_deleted = call_profile->arguments.count(SRCDIFF_DELETE);
                    size_t number_arguments_inserted = call_profile->arguments.count(SRCDIFF_INSERT);

                    size_t number_arguments_modified = 0;
                    std::for_each(call_profile->arguments.lower_bound(SRCDIFF_COMMON), call_profile->arguments.upper_bound(SRCDIFF_COMMON),
                        [&, this](const typename change_entity_map<profile_t>::pair & pair) {

                                if(pair.second->syntax_count) {

                                    bool report_change = false;
                                    for(const std::shared_ptr<profile_t> & argument_child_profile : pair.second->child_profiles[0]->child_profiles) {

                                        if(argument_child_profile->operation != SRCDIFF_COMMON) { 

                                                report_change = true;
                                                break;

                                        }

                                        if(argument_child_profile->type_name.is_common() && is_call(argument_child_profile->type_name)) {

                                            size_t num_calls = 0;
                                            std::vector<std::shared_ptr<call_profile_t>> inner_deleted_calls, inner_inserted_calls,
                                                inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists;
                                            expr_stmt_call(argument_child_profile, identifier_set, inner_deleted_calls, inner_inserted_calls,
                                                inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists);

                                            if(inner_deleted_calls.size() || inner_inserted_calls.size()
                                                || inner_modified_calls.size() || inner_renamed_calls.size() || inner_modified_argument_lists.size()) {

                                                report_change = true;
                                                break;

                                            }

                                        } else {

                                            if(!is_identifier(argument_child_profile->type_name)) {

                                                report_change = true;
                                                break;

                                            } else {

                                                const std::shared_ptr<identifier_profile_t> & identifier_profile
                                                    = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(argument_child_profile);

                                                identifier_diff ident_diff(identifier_profile->name);
                                                ident_diff.compute_diff();

                                                if(identifier_set.count(ident_diff.get_diff())) {

                                                    report_change = true;
                                                    break;

                                                }

                                            }

                                        }

                                    }

                                    if(report_change) ++number_arguments_modified; 

                                }


                            });

                    if(number_arguments_deleted == 0 && number_arguments_inserted == 0 && number_arguments_modified == 0)
                        report_argument_list = false;

                }

                if(!report_name && !report_argument_list) continue;

                modified_calls.push_back(call_profile);
                if(report_name) renamed_calls.push_back(call_profile);
                if(report_argument_list) modified_argument_lists.push_back(call_profile);

            }

         }

     }

}

summary_output_stream & text_summary::expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) const {

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    if(expr_stmt_profile->assignment() || expr_stmt_profile->is_delete() || profile->child_profiles.empty()) {

        out.begin_line() << get_profile_string(profile) << " was ";

        out << (profile->operation == SRCDIFF_DELETE ?  "deleted\n" : (profile->operation == SRCDIFF_INSERT ? "added\n" : "modified\n"));

        return out;

    }

    // may no longe be needed
    if(profile->child_profiles.empty()) return out;

    // this needs flushed out
    if(profile->operation != SRCDIFF_COMMON) {

        std::list<std::string> deleted_calls;
        std::list<std::string> inserted_calls;
        for(const std::shared_ptr<profile_t> & child_profile : profile->child_profiles[0]->child_profiles) {

            if(is_call(child_profile->type_name)) {

                const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_profile);

                if(profile->operation == SRCDIFF_DELETE)
                    deleted_calls.push_back(call_profile->name.original());
                else
                    inserted_calls.push_back(call_profile->name.modified());

            }

        }

        if(!deleted_calls.empty() || !inserted_calls.empty()) {

            out.begin_line();

            std::list<std::string> & call_names = !deleted_calls.empty() ? deleted_calls : inserted_calls;

            if(call_names.size() == 1) {

                out << "a call to '" << call_names.front() << "' was ";

                out << (profile->operation == SRCDIFF_DELETE ? "removed\n" : "added\n");

            } else {

                out << "calls to ";

                std::string ending = call_names.size() == 2 ? "' " : "', ";
                while(call_names.size() != 1) {

                    out << '\'' << call_names.front() << ending;
                    call_names.pop_front();

                }

                out << "and \'" << call_names.front() << "' were ";

                out << (profile->operation == SRCDIFF_DELETE ? "removed\n" : "added\n");

            }

        }

    } else {

        // there is probably a better way
        const std::shared_ptr<profile_t> & parent_profile = profile->parent;
        std::map<versioned_string, size_t> diff_set;
        std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
                            output_identifiers.begin(), output_identifiers.end(),
                            std::inserter(diff_set, diff_set.begin()));

        std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls,
            modified_calls, renamed_calls, modified_argument_lists;
        expr_stmt_call(profile, diff_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists);

        if(modified_calls.size() == 0) return out;

        out.begin_line();

        if(deleted_calls.size()) {

            if(deleted_calls.size() == 1) out << "a call to ";
            else                          out << "calls to ";

            for(size_t pos = 0; pos < deleted_calls.size(); ++pos) {

                out << '\'' << deleted_calls[pos]->name.original() << '\'';

                if(deleted_calls.size() == 2 && pos == 0)
                    out << " and ";
                else if(deleted_calls.size() > 2 && pos != (deleted_calls.size() - 1))
                    out << ", ";
                else if(deleted_calls.size() > 2)
                    out << ", and ";

            }

            if(deleted_calls.size() == 1) out << " was deleted from a call chain";
            else                          out << " were deleted from a call chain";

            if(inserted_calls.size() || modified_calls.size())
                out << " and ";

        }

        if(inserted_calls.size()) {

            if(inserted_calls.size() == 1) out << "a call to ";
            else                          out << "calls to ";

            for(size_t pos = 0; pos < inserted_calls.size(); ++pos) {

                out << '\'' << inserted_calls[pos]->name.modified() << '\'';

                if(inserted_calls.size() == 2 && pos == 0)
                    out << " and ";
                else if(inserted_calls.size() > 2 && pos != (inserted_calls.size() - 1))
                    out << ", ";
                else if(inserted_calls.size() > 2)
                    out << ", and ";

            }

            if(inserted_calls.size() == 1) out << " was added to a call chain";
            else                           out << " were added to a call chain";

            if(modified_calls.size())
                out << " and ";

        }

        if(renamed_calls.size() && modified_argument_lists.size()) {

            if(modified_calls.size() == 1) {

                out << get_profile_string(renamed_calls[0]) << " and its arguments modified\n";

            } else {
             
                if(renamed_calls.size() == 1) out << get_profile_string(renamed_calls[0]);
                else                    out << renamed_calls.size() << " function calls were renamed";
                out << " and ";

                if(modified_argument_lists.size() == 1) out << "a function call's arguments ";
                else                                   out << renamed_calls.size() << " function calls' argument lists ";
                out << " were modified\n";

            }

        } else if(renamed_calls.size()) {

            if(renamed_calls.size() == 1) out << get_profile_string(renamed_calls[0]) << '\n';
            else                    out << renamed_calls.size() << " function calls were renamed\n";

        } else if(modified_argument_lists.size()) {

            if(modified_argument_lists.size() == 1) out << "a function call's arguments";
            else                                   out << renamed_calls.size() << " function calls' argument lists";
            out << " were modified\n";

        }

    }

     return out;

}

summary_output_stream & text_summary::decl_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) const {

    const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);

    out.begin_line() << get_profile_string(decl_stmt_profile);

    out << " was ";

    out << (profile->operation == SRCDIFF_DELETE ?  "deleted\n" : (profile->operation == SRCDIFF_INSERT ? "added\n" : "modified\n"));

    return out;

}

summary_output_stream & text_summary::else_clause(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) {

    const bool has_common = profile->common_profiles.size() > 0;

    if(profile->parent->operation != SRCDIFF_COMMON) {

        out.begin_line();
        out << "an if statement with an else-clause was ";
        out << (profile->parent->operation == SRCDIFF_DELETE ? "removed" : "added");
        out << '\n';

    }

    out.begin_line();

    if(profile->parent->operation != SRCDIFF_COMMON)
        out << "the " << get_type_string(profile) << " was ";
    else
        out << get_profile_string(profile) << " was ";

    if(profile->operation != SRCDIFF_COMMON)
         out << (profile->operation == SRCDIFF_DELETE ? "removed" : "added");
    else out << "modified";

    if(profile->operation != SRCDIFF_COMMON && has_common) {

        if(profile->operation == SRCDIFF_DELETE)
            out << " from around ";
        else
            out << " around ";

        std::string common_summary;
        if(profile->common_profiles.size() == 1) {

            const std::shared_ptr<profile_t> & common_profile = profile->common_profiles.back();
            out <<  get_article(common_profile) << ' ';
            common_summary = get_type_string(common_profile);

        } else {

            common_summary = "existing code";

        }

        out << common_summary;
        
        if(profile->total_count != 0)  out << " and the " << common_summary << " was then modified ";

    }

    bool is_leaf = true;

    if(profile->summary_identifiers.size() > 0) {

        out << '\n';
        out.pad() << "  this modification included:\n";            
        is_leaf = false;
        out.increment_depth();
        identifiers(out, profile->summary_identifiers);
        out.decrement_depth();

    }

    /** todo should I only report if one expr_stmt modified, what if expression statement after condition both having been modified */
    for(size_t pos = 0; pos < profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        /** @todo check this condition */
        if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
             && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            if(is_leaf) {

                out << '\n';
                out.pad() << "  this modification included:\n";
                is_leaf = false;

            }

            out.increment_depth();

            if(child_profile->is_replacement && ((pos + 1) < profile->child_profiles.size())) {

                replacement(out, profile, pos);

            } else {

                if(is_condition_type(child_profile->type_name))
                    conditional(out, child_profile);
                else if(is_expr_stmt(child_profile->type_name))
                    expr_stmt(out, child_profile);
                else if(is_decl_stmt(child_profile->type_name))
                    decl_stmt(out, child_profile);

            }

            out.decrement_depth();

        }

    }

    // after children
    if(is_leaf) {

        if(profile->parent == id && (profile->operation == SRCDIFF_COMMON || !has_common)) {

            if(profile->operation == SRCDIFF_DELETE)      out << " from ";
            else if(profile->operation == SRCDIFF_INSERT) out << " to ";
            else                                          out << " within ";

            out << "the function body";

        }

        out << '\n';

    }

    return out;

}

summary_output_stream & text_summary::conditional(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) {

    const bool has_common = profile->common_profiles.size() > 0;

    const std::shared_ptr<conditional_profile_t> & conditional_profile = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile);
    const bool condition_modified = conditional_profile->is_condition_modified();
    const bool body_modified = conditional_profile->is_body_modified();
    boost::optional<srcdiff_type> else_operation;
    if(profile->type_name == "if") else_operation = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->else_operation();
    const bool else_modified = bool(else_operation) && else_operation == SRCDIFF_COMMON;
    const versioned_string & condition = conditional_profile->get_condition();

    if(!condition_modified && !body_modified && bool(else_operation)
        && (profile->operation == SRCDIFF_COMMON || profile->child_profiles.back()->common_profiles.size() > 0))
        return else_clause(out, profile->child_profiles[0]);

    out.begin_line();

    // before children
    if(profile->operation == SRCDIFF_COMMON && (body_modified || condition_modified)) {

        out << "the ";

        if(condition_modified && body_modified && else_modified) out << "condition, body, and else-clause ";
        else if(condition_modified && body_modified)             out << "condition and body ";
        else if(condition_modified && else_modified)             out << "condition and else-clause ";
        else if(body_modified && else_modified)                  out << "body and else-clause ";
        else if(condition_modified)                              out << "condition ";
        else if(body_modified)                                   out << "body ";

        out << "of ";

    }

    out << get_profile_string(profile) << " was ";

    if(profile->operation != SRCDIFF_COMMON)
         out << (profile->operation == SRCDIFF_DELETE ? "removed" : "added");
    else out << "modified";

    if(profile->operation != SRCDIFF_COMMON && has_common) {

        if(profile->operation == SRCDIFF_DELETE)
            out << " from around ";
        else
            out << " around ";

        std::string common_summary;
        if(profile->common_profiles.size() == 1) {

            const std::shared_ptr<profile_t> & common_profile = profile->common_profiles.back();
            out <<  get_article(common_profile) << ' ';
            common_summary = get_type_string(common_profile);

        } else {

            common_summary = "existing code";

        }

        out << common_summary;
        
        if(profile->total_count != 0)  out << " and the " << common_summary << " was then modified ";

    }

    bool is_leaf = true;

    if(condition_modified) {

        out << '\n';
        out.pad() << "  this modification included:\n";            
        is_leaf = false;
        out.increment_depth();
        out.begin_line() << "the condition was changed from '" << condition.original() << "' to '" << condition.modified() << "'\n";
        out.decrement_depth();
    }

    if(profile->summary_identifiers.size() > 0) {

        out << '\n';
        out.pad() << "  this modification included:\n";            
        is_leaf = false;
        out.increment_depth();
        identifiers(out, profile->summary_identifiers);
        out.decrement_depth();

    }

    /** todo should I only report if one expr_stmt modified, what if expression statement after condition both having been modified */
    for(size_t pos = 0; pos < profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        /** @todo check this condition */
        if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
             && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            if(is_leaf) {

                out << '\n';
                out.pad() << "  this modification included:\n";
                is_leaf = false;

            }

            out.increment_depth();

            if(child_profile->is_replacement && ((pos + 1) < profile->child_profiles.size())) {

                replacement(out, profile, pos);

            } else {

                if(is_condition_type(child_profile->type_name))
                    conditional(out, child_profile);
                else if(is_expr_stmt(child_profile->type_name))
                    expr_stmt(out, child_profile);
                else if(is_decl_stmt(child_profile->type_name))
                    decl_stmt(out, child_profile);

            }

            out.decrement_depth();

        }

    }

    // after children
    if(is_leaf) {

        if(profile->parent == id && (profile->operation == SRCDIFF_COMMON || !has_common)) {

            if(profile->operation == SRCDIFF_DELETE)      out << " from ";
            else if(profile->operation == SRCDIFF_INSERT) out << " to ";
            else                                          out << " within ";

            out << "the function body";

        }

        out << '\n';

    }

    return out;

}

summary_output_stream & text_summary::body(summary_output_stream & out, const profile_t & profile) {

    identifiers(out, summary_identifiers);

    for(size_t pos = 0; pos < child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = child_profiles[pos];

        if(!is_body_summary(child_profile->type_name, child_profile->is_replacement) || (child_profile->operation == SRCDIFF_COMMON && child_profile->syntax_count == 0))
            continue;

        if(child_profile->is_replacement && ((pos + 1) < child_profiles.size())) {

            replacement(out, std::make_shared<profile_t>(profile), pos);

        } else {

            if(is_condition_type(child_profile->type_name))
                conditional(out, child_profile);
            else if(is_expr_stmt(child_profile->type_name))
                expr_stmt(out, child_profile);
            else if(is_decl_stmt(child_profile->type_name))
                decl_stmt(out, child_profile);

        }

    }

    return out;

}
