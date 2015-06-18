#ifndef INCLUDED_CALL_PROFILE_T_HPP
#define INCLUDED_CALL_PROFILE_T_HPP

#include <profile_t.hpp>

#include <identifier_profile_t.hpp>

#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

class call_profile_t : public profile_t {

    private:

    public:

        std::shared_ptr<identifier_profile_t> name;

  	    change_entity_map<profile_t> arguments;
        bool argument_list_modified;

        call_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
        	: profile_t(type_name, uri, operation, parent), argument_list_modified(false) {}

        virtual void set_name(const std::shared_ptr<profile_t> & name, const boost::optional<versioned_string> & parent) {

            if(is_call(*parent)) this->name = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(name);

        }

        virtual void add_child_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

        	if(is_argument(profile->type_name)) arguments.emplace(profile->operation, profile);

            child_change_profiles.insert(std::lower_bound(child_change_profiles.begin(), child_change_profiles.end(), profile), profile);
            
        }

        // virtual void add_common(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

        //     common_profiles.push_back(profile);

        // } 

};

#endif
