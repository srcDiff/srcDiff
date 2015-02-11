#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

struct unit_profile_t : public profile_t {

    private:

    public:

        versioned_string file_name;

        unit_profile_t(std::string type_name = "") : profile_t(type_name) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            file_name = name;

        }

};

#endif