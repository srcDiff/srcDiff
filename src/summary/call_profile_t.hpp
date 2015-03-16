#ifndef INCLUDED_CALL_PROFILE_T_HPP
#define INCLUDED_CALL_PROFILE_T_HPP

#include <profile_t.hpp>

#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

class call_profile_t : public profile_t {

    private:

    public:

        versioned_string name;

  	    change_entity_map<profile_t> arguments;
        bool argument_list_modified;

        call_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id)
        	: profile_t(type_name, uri, operation, parent_id), argument_list_modified(false) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            if(is_call(*parent)) this->name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

        	if(is_argument(profile->type_name)) arguments.emplace(profile->operation, profile);

            child_profiles.insert(std::lower_bound(child_profiles.begin(), child_profiles.end(), profile->id), profile->id);
            
        }

};

#endif
