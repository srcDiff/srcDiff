#ifndef INCLUDED_PARAMETER_PROFILE_T_HPP
#define INCLUDED_PARAMETER_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class parameter_profile_t : public profile_t {

    private:

    public:

        versioned_string type;
        versioned_string name;

        parameter_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : profile_t(type_name, uri, operation, parent_id) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "type")      type = name;
            else if(*parent == "decl") this->name = name;

        }

};

#endif
