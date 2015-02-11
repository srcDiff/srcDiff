#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <counts_t.hpp>

#include <vector>
#include <map>
#include <iostream>

struct function_profile_t : public profile_t {

    private:

    public:

        std::string return_type;
        std::string name;

        function_profile_t(std::string type_name = "") : profile_t(type_name) {}

};

#endif