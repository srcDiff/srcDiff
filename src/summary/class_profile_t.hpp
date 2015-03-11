#ifndef INCLUDED_CLASS_PROFILE_T_HPP
#define INCLUDED_CLASS_PROFILE_T_HPP

#include <profile_t.hpp>
#include <function_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

class class_profile_t : public profile_t {

    private:

    public:

        versioned_string name;

        change_entity_map<decl_stmt_profile_t> members;
        change_entity_map<function_profile_t>  methods;
        change_entity_map<class_profile_t>     classes;

    public:

        class_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : profile_t(type_name, uri, operation, parent_id) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            const std::string type_name = parent->is_common() ? std::string(*parent) : parent->original();

            if(is_class_type(type_name)) this->name = name;

        }

        virtual void add_descendant(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_decl_stmt(type_name))          members.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(type_name)) methods.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(type_name))    classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));

            descendant_profiles.insert(std::lower_bound(descendant_profiles.begin(), descendant_profiles.end(), profile->id), profile->id);
            
        }

        virtual std::ostream & summary(std::ostream & out, size_t summary_types, const profile_list_t & profile_list) const {

            begin_line(out) << type_name << " '" << name << "': Impact = " << get_impact_factor() << '\n'; 

            /** Change these to text and table summaries */
            members.summarize_pure(out, summary_types, profile_list, SRCDIFF_DELETE);
            members.summarize_pure(out, summary_types, profile_list, SRCDIFF_INSERT);
            members.summarize_modified(out, summary_types, profile_list);

            methods.summarize_pure(out, summary_types, profile_list, SRCDIFF_DELETE);
            methods.summarize_pure(out, summary_types, profile_list, SRCDIFF_INSERT);
            methods.summarize_modified(out, summary_types, profile_list);

            classes.summarize_pure(out, summary_types, profile_list, SRCDIFF_DELETE);
            classes.summarize_pure(out, summary_types, profile_list, SRCDIFF_INSERT);
            classes.summarize_modified(out, summary_types, profile_list);

            return out;

        }

};

#endif
