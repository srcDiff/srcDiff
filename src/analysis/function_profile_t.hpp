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

class function_profile_t : public profile_t, public conditionals_addon {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        change_entity_map<parameter_profile_t> parameters;

    public:

        function_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation), conditionals_addon() {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type") return_type = name;
            else if(is_function_type(*parent)) this->name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_parameter(type_name)) parameters.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, profile);
            else child_profiles.push_back(profile->id);

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

            pad(out) << "Parameter list changes:\n";

            ++depth;
            output_header(out);
            output_counts(out, "Parameters", number_parameters_deleted, number_parameters_inserted, number_parameters_modified);
            --depth;

            return out;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            pad(out) << type_name << " '" << name << "': Impact = " << get_impact_factor() << '\n'; 
            // out << " Whitespace: " << whitespace_count;
            // out << "\tComment: " << comment_count;
            // out << "\tSyntax: " << syntax_count;
            // out << "\tTotal: " << total_count;
            // out << '\n';

            ++depth;

            // function signature
            if(!name.is_common()) pad(out) << "Name changed: " << name.original() << " -> " << name.modified() << '\n';

            // behaviour change
            bool is_return_type_change = !return_type.is_common();
            size_t number_parameters_deleted  = parameters.count(SRCDIFF_DELETE);
            size_t number_parameters_inserted = parameters.count(SRCDIFF_INSERT);
            size_t number_parameters_modified = 0;
            std::for_each(parameters.find(SRCDIFF_COMMON), parameters.upper_bound(SRCDIFF_COMMON),
                [&number_parameters_modified](const change_entity_map<profile_t>::pair & pair) { if(pair.second->syntax_count) ++number_parameters_modified; });

            if(is_return_type_change || number_parameters_deleted || number_parameters_inserted || number_parameters_modified) pad(out) << "Signature change:\n";

            ++depth;

            if(is_return_type_change) pad(out) << "Return type changed: " << return_type.original() << " -> " << return_type.modified() << '\n';

            if(number_parameters_deleted || number_parameters_inserted || number_parameters_modified)
                output_all_parameter_counts(out, number_parameters_deleted, number_parameters_inserted, number_parameters_modified);

            --depth;

            // body summary

            /** @todo determine guards.  Might be if with only return break or continue probably needs to be detected in sax and set as part of profile will
                probably have to create an if_profile then when in then check and see if non-comment non-return/break/continue and then set no longer guard */
            size_t number_conditionals_deleted  = conditionals.count(SRCDIFF_DELETE);
            size_t number_conditionals_inserted = conditionals.count(SRCDIFF_INSERT);
            size_t number_conditionals_modified = 0;
            std::for_each(conditionals.lower_bound(SRCDIFF_COMMON), conditionals.upper_bound(SRCDIFF_COMMON),
                [&number_conditionals_modified](const change_entity_map<profile_t>::pair & pair) { if(pair.second->syntax_count) ++number_conditionals_modified; });
            if(number_conditionals_deleted || number_conditionals_inserted || number_conditionals_modified)
                output_all_conditional_counts(out, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);

            --depth;

            return out;

        }

};

#endif
