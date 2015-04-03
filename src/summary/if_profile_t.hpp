#ifndef INCLUDED_IF_PROFILE_T_HPP
#define INCLUDED_IF_PROFILE_T_HPP

#include <conditional_profile_t.hpp>

class if_profile_t : public conditional_profile_t {

    private:

        bool guard;
        bool has_else;

    public:

        if_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) :
            conditional_profile_t(type_name, uri, operation, parent), guard(true), has_else(false) {}


        bool is_guard() const {

            return guard;

        }

        void is_guard(bool guard) {

            this->guard = guard;

        }

        bool else_clause() const {

            return has_else;

        }

        void else_clause(bool has_else) {

            this->has_else = has_else;

        }

};

#endif
