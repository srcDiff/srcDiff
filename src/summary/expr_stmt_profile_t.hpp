#ifndef INCLUDED_EXPR_STMT_PROFILE_T_HPP
#define INCLUDED_EXPR_STMT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class expr_stmt_profile_t : public profile_t {

    private:

    	bool is_assignment;
        bool isdelete;
        bool is_call;

        std::vector<std::shared_ptr<call_profile_t>> call_profiles;

        versioned_string left;
        versioned_string right;

    public:

        expr_stmt_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
            : profile_t(type_name, uri, operation, parent), is_assignment(false), isdelete(false), is_call(true), left(), right() {}

        bool assignment() const {

        	return is_assignment;

        }

        void set_assignment(bool is_assignment) {

        	this->is_assignment = is_assignment;

        }

        bool is_delete() const {

            return isdelete;

        }

        void set_delete(bool isdelete) {

            this->isdelete = isdelete;

        }

        bool call() const {

            return is_call;

        }

        void set_call(bool is_call) {

            this->is_call = is_call;

        }

        std::vector<std::shared_ptr<call_profile_t>> & get_call_profiles() {

            return call_profiles;

        }

        void add_call_profile(const std::shared_ptr<call_profile_t> & call_profile) {

            call_profiles.push_back(call_profile);

        }

        const versioned_string & lhs() const {

            return left;

        }

        void set_lhs(const versioned_string & left) {

            this->left = left;

        }

        const versioned_string & rhs() const {

            return right;

        }

        void set_rhs(const versioned_string & right) {

            this->right = right;

        }

};

#endif
