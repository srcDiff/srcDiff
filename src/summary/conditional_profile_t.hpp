#ifndef INCLUDED_CONDITIONAL_PROFILE_T_HPP
#define INCLUDED_CONDITIONAL_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class conditional_profile_t : public profile_t {

    private:

        bool condition_modified;
        bool body_modified;

        versioned_string condition;

    public:

        conditional_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : profile_t(type_name, uri, operation, parent_id),
                              condition_modified(false), body_modified(false) {}

        virtual void set_condition(versioned_string condition) {

            this->condition = condition;

        }

        void set_condition_modified(bool condition_modified) {

            this->condition_modified = condition_modified;

        }

        void set_body_modified(bool body_modified) {

            this->body_modified = body_modified;

        }

        const versioned_string & get_condition() const {

            return condition;

        }

        bool is_condition_modified() const {

            return condition_modified;

        }

        bool is_body_modified() const {

            return body_modified;

        }

};

#endif
