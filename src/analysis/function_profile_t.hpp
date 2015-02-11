#ifndef INCLUDED_FUNCTION_PROFILE_T_HPP
#define INCLUDED_FUNCTION_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

struct function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        function_profile_t(std::string type_name = "") : profile_t(type_name) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(*parent == "type") return_type = name;
            else this->name = name;

        }

        friend std::ostream & operator<<(std::ostream & out, const function_profile_t & profile) {

            out << profile.type_name << " '" << profile.name << "':"; 
            out << " Whitespace: " << profile.whitespace_count;
            out << "\tComment: " << profile.comment_count;
            out << "\tSyntax: " << profile.syntax_count;
            out << "\tTotal: " << profile.total_count;
            out << '\n';

            if(!profile.return_type.is_common()) out << "\tReturn type changed: " << profile.return_type.get_original() << " -> " << profile.return_type.get_modified() << '\n';

            return out;

        }

};

#endif