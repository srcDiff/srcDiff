#ifndef INCLUDED_TERNARY_PROFILE_T_HPP
#define INCLUDED_TERNARY_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class ternary_profile_t : public profile_t {

    private:

        std::shared_ptr<profile_t> condition_;
        std::shared_ptr<profile_t> then_clause_;
        std::shared_ptr<profile_t> else_clause_;

    public:

        ternary_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) : profile_t(type_name, uri, operation, parent) {}

        virtual void add_common(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            if(profile->type_name == "else") else_clause_ = profile;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            if(profile->type_name == "else") else_clause_ = profile;

        }

        const std::shared_ptr<profile_t> & condition() const {

            return condition_;

        }

        void condition(const std::shared_ptr<profile_t> & condition_) {

            this->condition_ = condition_;

        }

        const std::shared_ptr<profile_t> & then_clause() const {

            return then_clause_;

        }

        void then_clause(const std::shared_ptr<profile_t> & then_clause_) {

            this->then_clause_ = then_clause_;

        }

        const std::shared_ptr<profile_t> & else_clause() const {

            return else_clause_;

        }

};

#endif
