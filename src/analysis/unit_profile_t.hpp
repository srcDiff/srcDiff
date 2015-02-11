#ifndef INCLUDED_UNIT_PROFILE_T_HPP
#define INCLUDED_UNIT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

struct unit_profile_t : public profile_t {

    private:

    public:

        versioned_string file_name;

        unit_profile_t(std::string type_name = "") : profile_t(type_name) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            fprintf(stderr, "HERE: %s %s %d '%s'\n", __FILE__, __FUNCTION__, __LINE__, name.get_original().c_str());

            file_name = name;

        }

        friend std::ostream & operator<<(std::ostream & out, const unit_profile_t & profile) {

            out << profile.type_name << " '" << profile.file_name << "':"; 
            out << " Whitespace: " << profile.whitespace_count;
            out << "\tComment: " << profile.comment_count;
            out << "\tSyntax: " << profile.syntax_count;
            out << "\tTotal: " << profile.total_count;
            out << '\n';

            return out;

        }

};

#endif