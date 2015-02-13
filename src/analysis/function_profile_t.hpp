#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <versioned_string.hpp>

#include <map>

struct function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        std::multimap<srcdiff_type, std::shared_ptr<parameter_profile_t>> parameters;

        function_profile_t(std::string type_name = "") : profile_t(type_name) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(*parent == "type") return_type = name;
            else this->name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile, srcdiff_type operation) {

            if(profile->type_name == "parameter") parameters.emplace(operation, reinterpret_cast<const std::shared_ptr<parameter_profile_t> &>(profile));
            else child_profiles.push_back(profile->id);

        }

        virtual std::ostream & summary(std::ostream & out) const {

            out << type_name << " '" << name << "':"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            // function signature
            if(!name.is_common()) out << "\tName changed: " << name.original() << " -> " << name.modified() << '\n';

            // behaviour change
            bool is_return_type_change = !return_type.is_common();
            size_t num_deleted_parameters = parameters.count(SRCDIFF_DELETE);
            size_t num_inserted_parameters = parameters.count(SRCDIFF_INSERT);
            size_t num_modified_parameters = parameters.count(SRCDIFF_COMMON);
            //if(is_return_type_change || num_deleted_parameters || num_inserted_parameters || num_modified_parameters) out << "\tThe following indicate a change of behaviour to the function:\n";

            if(is_return_type_change)   out << "\t\tReturn type changed: " << return_type.original() << " -> " << return_type.modified() << '\n';
            if(num_deleted_parameters)  out << "\t\tNumber deleted parameters: " << num_deleted_parameters << '\n';
            if(num_inserted_parameters) out << "\t\tNumber inserted parameters: " << num_inserted_parameters << '\n';
            if(num_modified_parameters) out << "\t\tNumber modified parameters: " << num_modified_parameters << '\n';

            return out;

        }

};

#endif