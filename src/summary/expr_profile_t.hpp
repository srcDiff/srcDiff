#ifndef INCLUDED_EXPR_PROFILE_T_HPP
#define INCLUDED_EXPR_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class expr_profile_t : public profile_t {

    private:

    	bool has_assignment;
        bool has_delete;

    public:

        expr_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
            : profile_t(type_name, uri, operation, parent), has_assignment(false), has_delete(false) {}

        bool assignment() const {

        	return has_assignment;

        }

        void assignment(bool has_assignment) {

        	this->has_assignment = has_assignment;

        }

        bool is_delete() const {

            return has_delete;

        }

        void is_delete(bool has_delete) {

            this->has_delete = has_delete;

        }


};

#endif
