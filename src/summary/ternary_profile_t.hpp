#ifndef INCLUDED_TERNARY_PROFILE_T_HPP
#define INCLUDED_TERNARY_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class ternary_profile_t : public profile_t {

    private:

        std::shared_ptr<expr_profile_t> condition_;
        std::shared_ptr<expr_profile_t> then_clause_;
        std::shared_ptr<expr_profile_t> else_clause_;

    public:

        ternary_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) : profile_t(type_name, uri, operation, parent) {}

        virtual void add_common(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

            if(is_expr(profile->type_name)) {

                if(parent == "condition") condition_   = reinterpret_cast<const std::shared_ptr<expr_profile_t> &>(profile);
                else if(parent == "then") then_clause_ = reinterpret_cast<const std::shared_ptr<expr_profile_t> &>(profile);
                else if(parent == "else") else_clause_ = reinterpret_cast<const std::shared_ptr<expr_profile_t> &>(profile);
                else common_profiles.push_back(profile);
            }

            common_profiles.push_back(profile);

        }

        const std::shared_ptr<expr_profile_t> & condition() const {

            return condition_;

        }

        const std::shared_ptr<expr_profile_t> & then_clause() const {

            return then_clause_;

        }

        const std::shared_ptr<expr_profile_t> & else_clause() const {

            return else_clause_;

        }

};

#endif
