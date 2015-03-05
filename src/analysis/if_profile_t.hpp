#ifndef INCLUDED_IF_PROFILE_T_HPP
#define INCLUDED_IF_PROFILE_T_HPP

#include <conditional_profile_t.hpp>

class if_profile_t : public conditional_profile_t {

    private:

        bool guard;

    public:

        if_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : conditional_profile_t(type_name, uri, operation, parent_id), guard(true) {}


        bool is_guard() const {

            return guard;

        }

        void set_is_guard(bool guard) {

            this->guard = guard;

        }

};

#endif
