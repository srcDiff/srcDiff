#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

#include <map>

struct function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        std::multimap<srcdiff_type, std::shared_ptr<profile_t>> parameters;

        function_profile_t(std::string type_name = "") : profile_t(type_name) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(*parent == "type") return_type = name;
            else this->name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile, srcdiff_type operation) {

            if(profile->type_name == "parameter") parameters.emplace(operation, profile);
            else child_profiles.push_back(profile->id);

        }

        virtual std::ostream & summary(std::ostream & out) const {

            out << type_name << " '" << name << "':"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            if(!return_type.is_common()) out << "\tReturn type changed: " << return_type.get_original() << " -> " << return_type.get_modified() << '\n';
            if(!name.is_common()) out << "\tName changed: " << name.get_original() << " -> " << name.get_modified() << '\n';

            return out;

        }

};

#endif