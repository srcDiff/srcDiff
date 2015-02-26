#ifndef INCLUDED_IF_PROFILE_T_HPP
#define INCLUDED_IF_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>
#include <type_query.hpp>

class if_profile_t : public profile_t {

    private:

        bool guard;

    public:

        if_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation), guard(true) {}


        bool is_guard() const {

            return guard;

        }

        void set_is_guard(bool guard) {

            this->guard = guard;

        }

};

#endif
