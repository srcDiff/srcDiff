#ifndef INCLUDED_EXPR_STMT_PROFILE_T_HPP
#define INCLUDED_EXPR_STMT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class expr_stmt_profile_t : public profile_t {

    private:

    	bool is_assignment;
        bool isdelete;
        bool is_call;

    public:

        expr_stmt_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id)
            : profile_t(type_name, uri, operation, parent_id), is_assignment(false), isdelete(false), is_call(true) {}

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

};

#endif
