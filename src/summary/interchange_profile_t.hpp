#ifndef INCLUDED_INTERCHANGE_PROFILE_T_HPP
#define INCLUDED_INTERCHANGE_PROFILE_T_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

class interchange_profile_t : public profile_t {

    private:

        std::shared_ptr<profile_t> original_;
        std::shared_ptr<profile_t> modified_;

    public:

        interchange_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
            : profile_t(type_name, uri, operation, parent), original_(), modified_() {}


        const std::shared_ptr<profile_t> & original() const {

            return original_;

        }

        const std::shared_ptr<profile_t> & modified() const {

            return modified_;

        }

        std::shared_ptr<profile_t> & original() {

            return original_;

        }

        std::shared_ptr<profile_t> & modified() {

            return modified_;

        }
};

#endif
