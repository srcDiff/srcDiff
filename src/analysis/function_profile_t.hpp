#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

#include <map>

class function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        change_entity_map<parameter_profile_t> parameters;
        change_entity_map<profile_t>           conditionals;

    public:

        function_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type") return_type = name;
            else this->name = name;

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
            size_t num_deleted_parameters  = parameters.count(SRCDIFF_DELETE);
            size_t num_inserted_parameters = parameters.count(SRCDIFF_INSERT);
            size_t num_modified_parameters = parameters.count(SRCDIFF_COMMON);
            if(is_return_type_change || num_deleted_parameters || num_inserted_parameters || num_modified_parameters) pad(out) << "Signature change:\n";//"\tThe following indicate a change of behaviour to the function:\n";

            ++depth;

            if(is_return_type_change)   pad(out) << "Return type changed: " << return_type.original() << " -> " << return_type.modified() << '\n';
            if(num_deleted_parameters)  pad(out) << "Number deleted parameters: " << num_deleted_parameters << '\n';
            if(num_inserted_parameters) pad(out) << "Number inserted parameters: " << num_inserted_parameters << '\n';
            if(num_modified_parameters) pad(out) << "Number modified parameters: " << num_modified_parameters << '\n';

            --depth;

            // body summary
            size_t num_conditionals_deleted  = conditionals.count(SRCDIFF_DELETE);
            size_t num_conditionals_inserted = conditionals.count(SRCDIFF_INSERT);
            if(num_conditionals_deleted || num_conditionals_inserted) pad(out) << "Testing complexity change:\n";

            ++depth;

            if(num_conditionals_deleted) pad(out) << "Number conditionals deleted: " << num_conditionals_deleted << '\n';
            if(num_conditionals_inserted) pad(out) << "Number conditionals inserted: " << num_conditionals_inserted << '\n';

            size_t num_conditionals_modified = conditionals.count(SRCDIFF_COMMON);
            if(num_conditionals_modified) pad(out) << "Number conditionals modified: " << num_conditionals_modified << '\n';

            depth -= 2;

            return out;

        }

};

#endif
