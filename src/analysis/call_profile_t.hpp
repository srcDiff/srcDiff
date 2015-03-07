#ifndef INCLUDED_CALL_PROFILE_T_HPP
#define INCLUDED_CALL_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>
#include <type_query.hpp>

class call_profile_t : public profile_t {

    private:

    public:

        versioned_string name;

        call_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : profile_t(type_name, uri, operation, parent_id) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(is_call(*parent)) this->name = name;

        }

};

#endif
