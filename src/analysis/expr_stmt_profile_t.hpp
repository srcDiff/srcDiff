#ifndef INCLUDED_EXPR_STMT_PROFILE_T_HPP
#define INCLUDED_EXPR_STMT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class expr_stmt_profile_t : public profile_t {

    private:

    	bool is_assignment;

    public:

        expr_stmt_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id)
            : profile_t(type_name, uri, operation, parent_id), is_assignment(false) {}

        bool assignment() const {

        	return is_assignment;

        }

        void set_assignment(bool is_assignment) {

        	this->is_assignment = is_assignment;

        }


};

#endif
