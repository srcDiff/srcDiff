#ifndef INCLUDED_IDENTIFIER_PROFILE_T_HPP
#define INCLUDED_IDENTIFIER_PROFILE_T_HPP

#include <profile_t.hpp>

#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

class identifier_profile_t : public profile_t {

    private:

    public:

        versioned_string name;
        std::set<versioned_string> simple_names;

        bool is_simple;

  	    change_entity_map<profile_t> arguments;
        bool argument_list_modified;

        identifier_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent)
        	: profile_t(type_name, uri, operation, parent), name(), simple_names(), is_simple(true), argument_list_modified(false) {}


};

#endif
