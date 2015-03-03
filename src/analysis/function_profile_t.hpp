#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <conditionals_addon.hpp>
#include <parameter_profile_t.hpp>
#include <if_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

#include <map>
#include <iomanip>
#include <functional>
#include <algorithm>

class function_profile_t : public profile_t, public conditionals_addon {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        boost::optional<srcdiff_type> const_specifier;

        change_entity_map<parameter_profile_t> parameters;
        change_entity_map<profile_t>           member_initializations;

    public:

        function_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : profile_t(type_name, uri, operation, parent_id), conditionals_addon() {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type") return_type = name;
            else if(is_function_type(*parent)) this->name = name;

        }

        virtual void add_descendant(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_parameter(type_name)) parameters.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, profile);
            else if(is_call(type_name) && parent == "member_init_list") member_initializations.emplace(profile->operation, profile);
            else if(is_specifier(type_name) && parent == "function") const_specifier = profile->operation;
            
            descendant_profiles.insert(std::lower_bound(descendant_profiles.begin(), descendant_profiles.end(), profile->id), profile->id);

        }

        virtual impact_factor calculate_impact_factor() const {

            size_t weak_modification = (return_type.is_common() ? 0 : 1) + (name.is_common() ? 0 : 1) + parameters.count(SRCDIFF_COMMON);

            size_t feature_modifications = parameters.count(SRCDIFF_DELETE) + parameters.count(SRCDIFF_INSERT);

            size_t behaviour_modifications = conditionals.count(SRCDIFF_COMMON) + conditionals.count(SRCDIFF_DELETE) + conditionals.count(SRCDIFF_INSERT);

            if((weak_modification + feature_modifications + behaviour_modifications) == 0) return NONE;
            if((feature_modifications + behaviour_modifications) ==  0)                    return LOW;
            if(feature_modifications < 3 && behaviour_modifications < 2)                   return MEDIUM;
            return HIGH;

        }

        virtual std::ostream & output_all_parameter_counts(std::ostream & out, size_t number_parameters_deleted, size_t number_parameters_inserted, size_t number_parameters_modified) const {

            out << '\n';
            begin_line(out) << "Parameter list changes:\n";

            ++depth;
            output_header(out);
            output_counts(out, "Parameters", number_parameters_deleted, number_parameters_inserted, number_parameters_modified);
            --depth;

            return out;

        }

        virtual std::ostream & output_all_member_initialization_counts(std::ostream & out, size_t number_initializations_deleted,
                                                                      size_t number_initializations_inserted, size_t number_initializations_modified) const {

            out << '\n';
            begin_line(out) << "Member intialization list changes:\n";

            ++depth;
            output_header(out);
            output_counts(out, "Init", number_initializations_deleted, number_initializations_inserted, number_initializations_modified);
            --depth;

            return out;

        }

        virtual std::ostream & summary(std::ostream & out, size_t summary_types, const profile_list_t & profile_list) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            begin_line(out) << type_name << " '" << name << "': Impact = " << get_impact_factor() << '\n'; 

            ++depth;

            if(syntax_count == 0) {

                size_t non_syntax_changes = whitespace_count + comment_count;

                begin_line(out) << "only ";

                if(non_syntax_changes == 1) out << "a single ";

                out << (whitespace_count != 0 ? "whitespace " : "");

                out << (whitespace_count != 0 && comment_count != 0 ? "and " : "");

                out << (comment_count != 0 ? "comment " : "") << "change";

                out << (non_syntax_changes == 1 ? "\n" : "s\n");

                return out;

            }

            // function signature
            if(!name.is_common()) begin_line(out) << "Name changed: " << name.original() << " -> " << name.modified() << '\n';

            // behaviour change
            bool is_return_type_change = !return_type.is_common();
            size_t number_parameters_deleted = 0, number_parameters_inserted = 0, number_parameters_modified = 0;
            count_operations(parameters, number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

            if(is_return_type_change || number_parameters_deleted || number_parameters_inserted || number_parameters_modified) begin_line(out) << "Signature change:\n";

            ++depth;

            if(is_return_type_change) begin_line(out) << "Return type changed: " << return_type.original() << " -> " << return_type.modified() << '\n';

            if(number_parameters_deleted || number_parameters_inserted || number_parameters_modified)
                output_all_parameter_counts(out, number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

            // before block summary
            /** @todo may need to add rest of things that can occur here between parameter list and block */
            if(const_specifier) begin_line(out) << (*const_specifier == SRCDIFF_DELETE ? "Deleted " : (*const_specifier == SRCDIFF_INSERT ? "Inserted " : "Moved ")) << "const specifier \n";

            size_t number_member_initializations_deleted = 0, number_member_initializations_inserted = 0, number_member_initializations_modified = 0;
            count_operations(member_initializations, number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);
            if(number_member_initializations_deleted || number_member_initializations_inserted || number_member_initializations_modified)
                output_all_member_initialization_counts(out, number_member_initializations_deleted, number_member_initializations_inserted, number_member_initializations_modified);
            --depth;

            if(is_summary_type(summary_types, summary_type::TEXT)) {

                // body summary
                for(size_t profile_pos : descendant_profiles) {

                    const std::shared_ptr<profile_t> & profile = profile_list[profile_pos];

                    if(!is_condition_type(profile->type_name) || (profile->operation == SRCDIFF_COMMON && profile->syntax_count == 0))
                        continue;

                    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
                    const bool has_common = profile->has_common;

                    if(profile->parent_id == id) {

                        if(profile->operation == SRCDIFF_COMMON) continue;

                        if(is_guard_clause) begin_line(out) << "guard clause was ";
                        else                begin_line(out) << profile->type_name << " statement was ";

                        out << (profile->operation == SRCDIFF_DELETE ? "removed from " : (has_common ? "added " : "added to "));

                        if(has_common) out << " around existing code in ";

                        out << "function\n";

                    } else {

                        /** todo so if parent is deleted/inserted then should report as part of base or new document or say context */
                        std::function<std::string (const std::shared_ptr<profile_t> & profile)> get_article 
                            = [](const std::shared_ptr<profile_t> & profile) 
                        { 

                            const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
                            if(is_guard_clause) return "a";

                            const char letter = std::string(profile->type_name)[0];

                            if(letter == 'a' || letter == 'i' || letter == 'o' || letter == 'u')
                                return "an";
                            else
                                return "a";
                        };

                        const std::shared_ptr<profile_t> & parent_profile = profile_list[profile->parent_id];
                        const bool is_parent_guard_clause = parent_profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(parent_profile)->is_guard() : false;
                        const bool has_common = profile->has_common;

                        if(profile->operation != SRCDIFF_COMMON) {

                            if(is_parent_guard_clause) begin_line(out) << "guard clause was modified ";
                            else                       begin_line(out) << parent_profile->type_name << " statement was modified ";

                            out << (profile->operation == SRCDIFF_DELETE ? "removing " : "adding ");

                            out << get_article(profile) << ' ';

                            if(is_guard_clause) out << "guard clause";
                            else                out << profile->type_name << " statement";

                            if(has_common) out << (profile->operation == SRCDIFF_DELETE ? " from " : " ") << "around existing code";

                            out << '\n';

                        } else {

                            begin_line(out);

                            if(parent_profile->operation != SRCDIFF_COMMON) out << "common ";

                            if(is_guard_clause) out << "guard clause within ";
                            else                out << profile->type_name << " statement within ";

                            if(parent_profile->operation != SRCDIFF_COMMON) out << (parent_profile->operation == SRCDIFF_DELETE ? "the deleted " : "the inserted ");
                            else if(!is_parent_guard_clause) out << get_article(parent_profile) << ' ';

                            if(is_parent_guard_clause) out << " guard clause ";
                            else                       out << parent_profile->type_name << " statement ";

                            out << "was modified\n";

                        }

                    }

                }

            }

            if(!is_summary_type(summary_types, summary_type::TABLE)) return out;

            size_t number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified = 0;
            count_operations(conditionals, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);
            if(number_conditionals_deleted || number_conditionals_inserted || number_conditionals_modified)
                output_all_conditional_counts(out, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);

            --depth;

            return out;

        }

};

#endif
