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

        class_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            if(is_class_type(*parent)) name = name;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            if(is_decl_stmt(profile->type_name))          members.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(profile->type_name)) methods.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(profile->type_name))    classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));
            else child_profiles.push_back(profile->id);
            
        }

        virtual std::ostream & summary(std::ostream & out) const {

            pad(out) << type_name << " '" << name << "':"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            //++depth;

            members.summarize_pure(out, SRCDIFF_DELETE);
            members.summarize_pure(out, SRCDIFF_INSERT);
            members.summarize_modified(out);

            methods.summarize_pure(out, SRCDIFF_DELETE);
            methods.summarize_pure(out, SRCDIFF_INSERT);
            methods.summarize_modified(out);

            classes.summarize_pure(out, SRCDIFF_DELETE);
            classes.summarize_pure(out, SRCDIFF_INSERT);
            classes.summarize_modified(out);

            //--depth;

            return out;

        }

};

#endif