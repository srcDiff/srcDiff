#ifndef INCLUDED_EXPR_STMT_PROFILE_T_HPP
#define INCLUDED_EXPR_STMT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class expr_stmt_profile_t : public profile_t {

    private:

    	bool is_assignment;
        bool is_delete_;
        bool is_call;
        bool is_print;

        std::vector<std::shared_ptr<call_profile_t>> call_profiles;

        versioned_string left;
        versioned_string right;

    public:

        expr_stmt_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
            : profile_t(type_name, uri, operation, parent), is_assignment(false), is_delete_(false), is_call(true), is_print(false), left(), right() {}

        bool assignment() const {

        	return is_assignment;

        }

        void assignment(bool is_assignment) {

        	this->is_assignment = is_assignment;

        }

        bool is_delete() const {

            return is_delete_;

        }

        void is_delete(bool is_delete_) {

            this->is_delete_ = is_delete_;

        }

        bool call() const {

            return is_call;

        }

        void call(bool is_call) {

            this->is_call = is_call;

        }

        bool print() const {

            return is_print;

        }

        void print(bool is_print) {

            this->is_print = is_print;

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

        void lhs(const versioned_string & left) {

            this->left = left;

        }

        const versioned_string & rhs() const {

            return right;

        }

        void rhs(const versioned_string & right) {

            this->right = right;

        }

};

#endif
