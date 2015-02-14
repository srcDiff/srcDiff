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

        function_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(*parent == "type") return_type = name;
            else this->name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            if(is_parameter(profile->type_name)) parameters.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else if(is_condition_type(profile->type_name)) conditionals.emplace(profile->operation, profile);
            else child_profiles.push_back(profile->id);

        }

        virtual std::ostream & summary(std::ostream & out) const {

            if(operation != SRCDIFF_COMMON) {

                out << '\'' << (name.has_original() ? name.original() : name.modified()) << '\'';
                return out;

            }

            pad(out) << type_name << " '" << name << "':" << " Impact Low/High?"; 
            // out << " Whitespace: " << whitespace_count;
            // out << "\tComment: " << comment_count;
            // out << "\tSyntax: " << syntax_count;
            // out << "\tTotal: " << total_count;
            out << '\n';

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