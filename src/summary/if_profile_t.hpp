#ifndef INCLUDED_IF_PROFILE_T_HPP
#define INCLUDED_IF_PROFILE_T_HPP

#include <conditional_profile_t.hpp>

class if_profile_t : public conditional_profile_t {

    private:

        bool guard;
        bool has_else;
        bool has_elseif;
        boost::optional<srcdiff_type> else_operation_type;

    public:

        if_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) :
            conditional_profile_t(type_name, uri, operation, parent), guard(true),
            has_else(false), else_operation_type(boost::optional<srcdiff_type>()), has_elseif(false) {}


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

        boost::optional<srcdiff_type> else_operation() const {

            return else_operation_type;

        }

        void else_operation(srcdiff_type else_type) {

            this->else_operation_type = else_type;

        }

        bool elseif_clause() const {

            return has_elseif;

        }

        void elseif_clause(bool has_elseif) {

            this->has_elseif = has_elseif;

        }

};

#endif
