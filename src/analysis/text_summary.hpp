#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>

#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

std::ostream & parameter_text_summary(std::ostream & out, size_t number_parameters_deleted,
                                      size_t number_parameters_inserted, size_t number_parameters_modified) const {

    if(number_parameters_deleted > 0) {

        begin_line(out);

        if(number_parameters_deleted == 1) {

            change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_DELETE);
            out << "the parameter '" << param_iterator->second->name.original() << "' of type '" << param_iterator->second->type.original() << "' was removed\n";

        } else {

            out << "the following parameters were removed:\n";

            ++depth;
            std::for_each(parameters.lower_bound(SRCDIFF_DELETE), parameters.upper_bound(SRCDIFF_DELETE),
                [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                    begin_line(out) << '\'' << parameter.second->name.original() << "' of type '" << parameter.second->type.original() << "'\n";

                });
            --depth;

        }

    }

    if(number_parameters_inserted > 0) {

        begin_line(out);

        if(number_parameters_inserted == 1) {

            change_entity_map<parameter_profile_t>::const_iterator param_iterator = parameters.find(SRCDIFF_INSERT);
            out << "the parameter '" << param_iterator->second->name.modified() << "' of type '" << param_iterator->second->type.modified() << "' was added\n";

        } else {

            out << "the following parameters were added:\n";

            ++depth;
            std::for_each(parameters.lower_bound(SRCDIFF_INSERT), parameters.upper_bound(SRCDIFF_INSERT),
                [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                    begin_line(out) << '\'' << parameter.second->name.modified() << "' of type '" << parameter.second->type.modified() << "'\n";

                });
            --depth;

        }

    }

    if(number_parameters_modified > 0) {

        begin_line(out);

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

            ++depth;
            std::for_each(parameters.lower_bound(SRCDIFF_COMMON), parameters.upper_bound(SRCDIFF_COMMON),
                [&out](const change_entity_map<parameter_profile_t>::pair & parameter) {

                    begin_line(out) << "the parameter ";
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
            --depth;

        }

    }

    return out;

}

std::ostream & member_initialization_text_summary(std::ostream & out, size_t number_member_initializations_deleted,
                                                       size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const {

    if(number_member_initializations_deleted > 0) {

        begin_line(out);

        if(number_member_initializations_deleted == 1) {

            change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_DELETE);
            out << "a member initilization for '" << member_iterator->second->name.original() << "' was removed\n";

        } else {

            out << "the following member initializations were removed:\n";

            ++depth;
            std::for_each(member_initializations.lower_bound(SRCDIFF_DELETE), member_initializations.upper_bound(SRCDIFF_DELETE),
                [&out](const change_entity_map<call_profile_t>::pair & member) {

                    begin_line(out) << '\'' << member.second->name.original() << "'\n";

                });
            --depth;

        }

    }

    if(number_member_initializations_inserted > 0) {

        begin_line(out);

        if(number_member_initializations_inserted == 1) {

            change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_INSERT);
            out << "a member initilization for '" << member_iterator->second->name.modified() << "' was added\n";

        } else {

            out << "the following member initializations were added:\n";

            ++depth;
            std::for_each(member_initializations.lower_bound(SRCDIFF_INSERT), member_initializations.upper_bound(SRCDIFF_INSERT),
                [&out](const change_entity_map<call_profile_t>::pair & member) {

                    begin_line(out) << '\'' << member.second->name.modified() << "'\n";

                });
            --depth;

        }

    }

    if(number_member_initializations_modified > 0) {

        begin_line(out);

        if(number_member_initializations_modified == 1) {

            change_entity_map<call_profile_t>::const_iterator member_iterator = member_initializations.find(SRCDIFF_COMMON);
            if(member_iterator->second->name.is_common())
                out << "the member initilization for '" << member_iterator->second->name << "' was modified\n";
            else
                out << "the name of a member initilization was changed from '" << member_iterator->second->name.original() << "' to '" << member_iterator->second->name.modified() << "'\n";

        } else {

            out << "the following member initializations were modified:\n";

            ++depth;
            std::for_each(member_initializations.lower_bound(SRCDIFF_COMMON), member_initializations.upper_bound(SRCDIFF_COMMON),
                [&out](const change_entity_map<call_profile_t>::pair & member) {

                    begin_line(out);
                    if(member.second->name.is_common())
                        out << '\'' << member.second->name << "'\n";
                    else
                        out << "the name of a member initilization was changed from '" << member.second->name.original() << "' to '" << member.second->name.modified() << "'\n";

                });
            --depth;

        }

    }

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

std::ostream & conditional_text_summary(std::ostream & out, const std::shared_ptr<profile_t> & profile, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
    const bool has_common = profile->has_common;

    begin_line(out);

    // before children
    if(profile->operation == SRCDIFF_COMMON) {

        const std::shared_ptr<conditional_profile_t> & conditional_profile = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile);

        out << "the ";

        if(conditional_profile->is_condition_modified()
            && conditional_profile->is_body_modified())       out << "condition and body ";
        else if(conditional_profile->is_condition_modified()) out << "condition ";
        else if(conditional_profile->is_body_modified())      out << "body ";

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
            out << " retaining ";
        else
            out << " around ";
        
        if(profile->total_count != 0)  out << "and modifying ";

        out << "its body";

    }

    bool is_leaf = true;
    for(size_t child_pos : profile->child_profiles) {

        const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

        /** @todo check this condition */
        if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
             && is_condition_type(child_profile->type_name)) {

            if(is_leaf) {

                out << '\n';
                pad(out) << "  this modification included:\n";
                is_leaf = false;

            }

            ++depth;
            conditional_text_summary(out, child_profile, profile_list);
            --depth;

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

std::ostream & conditional_text_summary(std::ostream & out, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

    for(size_t profile_pos : child_profiles) {

        const std::shared_ptr<profile_t> & profile = profile_list[profile_pos];

        if(!is_condition_type(profile->type_name) || (profile->operation == SRCDIFF_COMMON && profile->syntax_count == 0))
            continue;

            conditional_text_summary(out, profile, profile_list);

    }

    return out;

}

#endif