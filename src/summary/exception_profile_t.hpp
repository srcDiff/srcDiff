#ifndef INCLUDED_EXCEPTION_PROFILE_T_HPP
#define INCLUDED_EXCEPTION_PROFILE_T_HPP

#include <profile_t.hpp>

class exception_profile_t : public profile_t {

    private:

        size_t number_catches;

    public:

        exception_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) : profile_t(type_name, uri, operation, parent),
                            number_catches(0) {}


        void increment_catches() {

            ++number_catches;

        }

        size_t catches() const {

            return number_catches;

        }

};

#endif
