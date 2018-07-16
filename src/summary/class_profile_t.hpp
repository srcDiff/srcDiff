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

        class_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) : profile_t(type_name, uri, operation, parent) {}

        virtual const versioned_string & get_name() const {
            return name;
        }

        virtual const void * get_member(const std::string & type) {
            if(is_function_type(type))  return &methods;
            if(is_decl_stmt(type))      return &members;
            if(is_class_type(type))     return &classes;
            return nullptr;
        }

        virtual void set_name(const std::shared_ptr<identifier_profile_t> & name, const boost::optional<versioned_string> & parent) {

            const std::string type_name = parent->is_common() ? std::string(*parent) : parent->original();

            if(is_class_type(type_name)) this->name = name->name;

        }

        virtual void add_descendant_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_decl_stmt(type_name))          members.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(type_name)) methods.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(type_name))    classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));

            descendant_change_profiles.insert(std::lower_bound(descendant_change_profiles.begin(), descendant_change_profiles.end(), profile), profile);
            
        }

        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types) const {

            // out.begin_line() << type_name << " '" << name << "': Impact = " << get_impact_factor() << '\n'; 

            // /** Change these to text and table summaries */
            // members.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            // members.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            // members.summarize_modified(out, summary_types);

            methods.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            methods.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            methods.summarize_modified(out, summary_types);

            // classes.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            // classes.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            classes.summarize_modified(out, summary_types);

            return out;

        }

};

#endif
