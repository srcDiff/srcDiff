#ifndef INCLUDED_PARAMETER_PROFILE_T_HPP
#define INCLUDED_PARAMETER_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class parameter_profile_t : public profile_t {

    private:

    public:

        std::shared_ptr<profile_t> type;
        versioned_string name;

        std::shared_ptr<profile_t> init;

        parameter_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) : profile_t(type_name, uri, operation, parent) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(*parent == "decl") this->name = name;

        }

};

#endif
