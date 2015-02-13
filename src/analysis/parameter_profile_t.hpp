#ifndef INCLUDED_PARAMETER_PROFILE_T_HPP
#define INCLUDED_PARAMETER_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

struct parameter_profile_t : public profile_t {

    private:

    public:

        versioned_string type;
        versioned_string name;

        parameter_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(*parent == "type") type = name;
            else this->name = name;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            out << type_name << " '" << name << "':"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            // type
            if(!type.is_common()) out << "Parameter type change: " << type.original() << " -> " << type.modified();

            // name
            if(!name.is_common()) out << "Parameter name change: " << name.original() << " -> " << name.modified();

            return out;

        }

};

#endif