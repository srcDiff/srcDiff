#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

struct function_profile_t : public profile_t {

    private:

    public:

        versioned_string return_type;
        versioned_string name;

        function_profile_t(std::string type_name = "") : profile_t(type_name) {}

        void set_return_type(std::string return_type) {

            return_type = versioned_string(return_type);

        }

        virtual void set_name(versioned_string name, const std::string & parent) {

            if(parent == "type") return_type = name;
            else this->name = name;

        }

};

#endif