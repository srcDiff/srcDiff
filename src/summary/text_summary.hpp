#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <conditional_profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <call_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <if_profile_t.hpp>
#include <identifier_diff.hpp>
#include <change_entity_map.hpp>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <list>
#include <set>

class text_summary {

private:

protected:

        const size_t id;

        const std::vector<size_t> & child_profiles;

        const change_entity_map<parameter_profile_t> & parameters;
        const change_entity_map<call_profile_t>      & member_initializations;

        const std::map<versioned_string, size_t> & identifiers;
        const std::map<versioned_string, size_t> & intersecting_identifiers;
public:


    text_summary(const size_t id, const std::vector<size_t> & child_profiles, const change_entity_map<parameter_profile_t> & parameters,
                 const change_entity_map<call_profile_t> & member_initializations,
                 const std::map<versioned_string, size_t> & identifiers, 
                 const std::map<versioned_string, size_t> & intersecting_identifiers)
        : id(id), child_profiles(child_profiles), parameters(parameters), member_initializations(member_initializations),
          identifiers(identifiers), intersecting_identifiers(intersecting_identifiers) {}

    std::ostream & parameter(std::ostream & out, size_t number_parameters_deleted,
                                          size_t number_parameters_inserted, size_t number_parameters_modified) const {

        if(number_parameters_deleted > 0) {

            profile_t::begin_line(out);

            if(number_parameters_deleted == 1) {

                change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_DELETE);
                out << "the parameter '" << param_iterator->second->name.original() << "' of type '" << param_iterator->second->type.original() << "' was removed\n";

            } else {

                out << "the following parameters were removed:\n";

                ++profile_t::depth;
                std::for_each(parameters.lower_bound(SRCDIFF_DELETE), parameters.upper_bound(SRCDIFF_DELETE),
                    [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                        profile_t::begin_line(out) << '\'' << parameter.second->name.original() << "' of type '" << parameter.second->type.original() << "'\n";

                    });
                --profile_t::depth;

            }

        }

        if(number_parameters_inserted > 0) {

            profile_t::begin_line(out);

            if(number_parameters_inserted == 1) {

                change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_INSERT);
                out << "the parameter '" << param_iterator->second->name.modified() << "' of type '" << param_iterator->second->type.modified() << "' was added\n";

            } else {

                out << "the following parameters were added:\n";

                ++profile_t::depth;
                std::for_each(parameters.lower_bound(SRCDIFF_INSERT), parameters.upper_bound(SRCDIFF_INSERT),
                    [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                        profile_t::begin_line(out) << '\'' << parameter.second->name.modified() << "' of type '" << parameter.second->type.modified() << "'\n";

                    });
                --profile_t::depth;

            }

        }

        if(number_parameters_modified > 0) {

            profile_t::begin_line(out);

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

                ++profile_t::depth;
                std::for_each(parameters.lower_bound(SRCDIFF_COMMON), parameters.upper_bound(SRCDIFF_COMMON),
                    [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                        profile_t::begin_line(out) << "the parameter ";
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
                --profile_t::depth;

            }

        }

        return out;

    }

    std::ostream & member_initialization(std::ostream & out, size_t number_member_initializations_deleted,
                                                           size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const {

        if(number_member_initializations_deleted > 0) {

            profile_t::begin_line(out);

            if(number_member_initializations_deleted == 1) {

                change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_DELETE);
                out << "a member initilization for '" << member_iterator->second->name.original() << "' was removed\n";

            } else {

                out << "the following member initializations were removed:\n";

                ++profile_t::depth;
                std::for_each(member_initializations.lower_bound(SRCDIFF_DELETE), member_initializations.upper_bound(SRCDIFF_DELETE),
                    [&out](const change_entity_map<call_profile_t>::pair & member) {

                        profile_t::begin_line(out) << '\'' << member.second->name.original() << "'\n";

                    });
                --profile_t::depth;

            }

        }

        if(number_member_initializations_inserted > 0) {

            profile_t::begin_line(out);

            if(number_member_initializations_inserted == 1) {

                change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_INSERT);
                out << "a member initilization for '" << member_iterator->second->name.modified() << "' was added\n";

            } else {

                out << "the following member initializations were added:\n";

                ++profile_t::depth;
                std::for_each(member_initializations.lower_bound(SRCDIFF_INSERT), member_initializations.upper_bound(SRCDIFF_INSERT),
                    [&out](const change_entity_map<call_profile_t>::pair & member) {

                        profile_t::begin_line(out) << '\'' << member.second->name.modified() << "'\n";

                    });
                --profile_t::depth;

            }

        }

        if(number_member_initializations_modified > 0) {

            profile_t::begin_line(out);

            if(number_member_initializations_modified == 1) {

                change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_COMMON);
                if(member_iterator->second->name.is_common())
                    out << "the member initilization for '" << member_iterator->second->name << "' was modified\n";
                else
                    out << "the name of a member initilization was changed from '" << member_iterator->second->name.original() << "' to '" << member_iterator->second->name.modified() << "'\n";

            } else {

                out << "the following member initializations were modified:\n";

                ++profile_t::depth;
                std::for_each(member_initializations.lower_bound(SRCDIFF_COMMON), member_initializations.upper_bound(SRCDIFF_COMMON),
                    [&out](const change_entity_map<call_profile_t>::pair & member) {

                        profile_t::begin_line(out);
                        if(member.second->name.is_common())
                            out << '\'' << member.second->name << "'\n";
                        else
                            out << "the name of a member initilization was changed from '" << member.second->name.original() << "' to '" << member.second->name.modified() << "'\n";

                    });
                --profile_t::depth;

            }

        }

        return out;

    }

    std::ostream & expr_stmt(std::ostream & out, const std::shared_ptr<profile_t> & profile, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

        if(expr_stmt_profile->assignment()) {
    
            profile_t::begin_line(out) << "an assignment statement was ";

            out << (profile->operation == SRCDIFF_DELETE ?  "deleted\n" : (profile->operation == SRCDIFF_INSERT ? "added\n" : "modified\n"));

            return out;

        }

        if(child_profiles.empty()) return out;

        // this needs flushed out
        if(profile->operation != SRCDIFF_COMMON) {

            std::list<std::string> deleted_calls;
            std::list<std::string> inserted_calls;
            for(size_t child_pos : profile_list[profile->child_profiles[0]]->child_profiles) {

                const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

                if(is_call(child_profile->type_name)) {

                    const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_profile);

                    if(profile->operation == SRCDIFF_DELETE)
                        deleted_calls.push_back(call_profile->name.original());
                    else
                        inserted_calls.push_back(call_profile->name.modified());

                }

            }

            if(!deleted_calls.empty() || !inserted_calls.empty()) {

                profile_t::begin_line(out);

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
            std::map<versioned_string, size_t> diff_set;
            std::set_difference(intersecting_identifiers.begin(), intersecting_identifiers.end(),
                                profile->identifiers.begin(), profile->identifiers.end(),
                                std::inserter(diff_set, diff_set.begin()));

            std::map<versioned_string, size_t> new_set;
            std::set_intersection(profile->identifiers.begin(), profile->identifiers.end(),
                                diff_set.begin(), diff_set.end(),
                                std::inserter(new_set, new_set.begin()));

            size_t number_calls = 0;
            size_t number_renames = 0;
            size_t number_argument_list_modified = 0;
            for(size_t child_pos : profile_list[profile->child_profiles[0]]->child_profiles) {

                const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

                if(child_profile->operation == SRCDIFF_COMMON && child_profile->type_name.is_common() && is_call(child_profile->type_name)) {

                    const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_profile);

                    if(!call_profile->name.is_common()) {

                        identifier_diff ident_diff(call_profile->name);
                        ident_diff.compute_diff();

                        if(!new_set.count(ident_diff.get_diff())) continue;

                    }

                    if(call_profile->argument_list_modified) {

                        size_t number_arguments_deleted = call_profile->arguments.count(SRCDIFF_DELETE);
                        size_t number_arguments_inserted = call_profile->arguments.count(SRCDIFF_INSERT);

                        size_t number_arguments_modified = 0;
                        std::for_each(call_profile->arguments.lower_bound(SRCDIFF_COMMON), call_profile->arguments.upper_bound(SRCDIFF_COMMON),
                            [&number_arguments_modified](const typename change_entity_map<profile_t>::pair & pair)
                                {

                                    if(pair.second->syntax_count) {


                                        ++number_arguments_modified; 

                                    }


                                });

                    }

                    ++number_calls;
                    if(!call_profile->name.is_common())      ++number_renames;
                    if(call_profile->argument_list_modified) ++number_argument_list_modified;

                }

             }

             if(number_calls == 0) return out;

            profile_t::begin_line(out);
            if(number_renames && number_argument_list_modified) {

                if(number_calls == 1) {

                    out << "a function call was renamed and its arguments modified\n";

                } else {
                 
                    if(number_renames == 1) out << "a function call was ";
                    else                    out << number_renames << " function calls were ";
                    out << "renamed and ";

                    if(number_argument_list_modified == 1) out << "a function call's arguments ";
                    else                                   out << number_renames << " function calls' argument lists ";
                    out << " were modified\n";

                }

            } else if(number_renames) {

                if(number_renames == 1) out << "a function call was ";
                else                    out << number_renames << " function calls were ";
                out << "renamed\n";

            } else if(number_argument_list_modified) {

                if(number_argument_list_modified == 1) out << "a function call's arguments";
                else                                   out << number_renames << " function calls' argument lists";
                out << " were modified\n";

            }

        }

         return out;

    }

    std::ostream & decl_stmt(std::ostream & out, const std::shared_ptr<profile_t> & profile, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

        profile_t::begin_line(out) << "a declaration statement was ";

        out << (profile->operation == SRCDIFF_DELETE ?  "deleted\n" : (profile->operation == SRCDIFF_INSERT ? "added\n" : "modified\n"));

        return out;

    }

    std::string get_article(const std::shared_ptr<profile_t> & profile) const { 

        const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
        if(is_guard_clause) return "a";

        const char letter = std::string(profile->type_name)[0];

        if(letter == 'a' || letter == 'i' || letter == 'o' || letter == 'u')
            return "an";
        else
            return "a";
    }

    std::ostream & conditional(std::ostream & out, const std::shared_ptr<profile_t> & profile, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

        const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
        const bool has_common = profile->has_common;

        const bool condition_modified = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile)->is_condition_modified();
        const bool body_modified = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile)->is_body_modified();
        const versioned_string & condition = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile)->get_condition();

        profile_t::begin_line(out);

        // before children
        if(profile->operation == SRCDIFF_COMMON) {

            out << "the ";

            if(condition_modified && body_modified) out << "condition and body ";
            else if(condition_modified)             out << "condition ";
            else if(body_modified)                  out << "body ";

            out << "of ";

        }

        out << get_article(profile) << ' ';

        if(is_guard_clause) out << "guard clause was ";
        else                out << profile->type_name << " statement was ";


        if(profile->operation != SRCDIFF_COMMON)
             out << (profile->operation == SRCDIFF_DELETE ? "removed" : "added");
        else out << "modified";

        if(profile->operation != SRCDIFF_COMMON && has_common) {

            if(profile->operation == SRCDIFF_DELETE)
                out << " from around existing code ";
            else
                out << " around existing code ";
            
            if(profile->total_count != 0)  out << "and the existing code was then modified ";

        }

        bool is_leaf = true;

        if(condition_modified) {

            is_leaf = false;
            out << '\n';
            ++profile_t::depth;
            profile_t::begin_line(out) << "the condition was changed from '" << condition.original() << "' to '" << condition.modified() << "'\n";
            --profile_t::depth;
        }

        /** todo should I only report if one expr_stmt modified, what if expression statement after condition both having been modified */
        for(size_t child_pos : profile->child_profiles) {

            const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

            /** @todo check this condition */
            if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
                 && is_body_summary(child_profile->type_name)) {

                if(is_leaf) {

                    out << '\n';
                    profile_t::pad(out) << "  this modification included:\n";
                    is_leaf = false;

                }

                ++profile_t::depth;

                if(is_condition_type(child_profile->type_name))
                    conditional(out, child_profile, profile_list);
                else if(is_expr_stmt(child_profile->type_name))
                    expr_stmt(out, child_profile, profile_list);

                --profile_t::depth;

            }

        }

        // after children
        if(is_leaf) {

            if(profile->parent_id == id && (profile->operation == SRCDIFF_COMMON || !has_common)) {

                if(profile->operation == SRCDIFF_DELETE)      out << " from ";
                else if(profile->operation == SRCDIFF_INSERT) out << " to ";
                else                                          out << " within ";

                out << "the function body";

            }

            out << '\n';

        }

        return out;

    }

    bool is_body_summary(const std::string & type) const {

        return is_condition_type(type) || is_expr_stmt(type) || is_decl_stmt(type);

    }

    /** 
        need to look if there was a common rename going on and report that
        might want to have profiles for identifiers. Which have a versioned string for name.
        Then have set_name take identifier_profile_t maybe, but probably not.  Then looking for renames,
        would just recurse throught children looking for identifier profiles.

        This should probably be collected part of function_profile_t in set or map when parsing.

    */
    std::ostream & body(std::ostream & out, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

        for(std::pair<versioned_string, size_t> identifier : intersecting_identifiers) {

            if(identifier.second <= 1) continue;

            profile_t::begin_line(out) << "the identifier '";

            if(identifier.first.has_original()) out << identifier.first.original() << "' ";

            if(identifier.first.has_original() && identifier.first.has_modified()) out << " was replaced with '";

            if(identifier.first.has_modified()) out << identifier.first.modified() << "' ";

            if(identifier.first.has_original() && !identifier.first.has_modified())
                out << "was removed ";
            else if(!identifier.first.has_original() && identifier.first.has_modified())
                out << "was added ";

            out << "in several places\n";

        }

        for(size_t profile_pos : child_profiles) {

            const std::shared_ptr<profile_t> & profile = profile_list[profile_pos];

            if(!is_body_summary(profile->type_name) || (profile->operation == SRCDIFF_COMMON && profile->syntax_count == 0))
                continue;

            if(is_condition_type(profile->type_name))
                conditional(out, profile, profile_list);
            else if(is_expr_stmt(profile->type_name))
                expr_stmt(out, profile, profile_list);
            else if(is_decl_stmt(profile->type_name))
                decl_stmt(out, profile, profile_list);

        }

        return out;

    }

};

#endif