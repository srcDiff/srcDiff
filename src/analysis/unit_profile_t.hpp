#ifndef INCLUDED_UNIT_PROFILE_T_HPP
#define INCLUDED_UNIT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <function_profile_t.hpp>
#include <class_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>

class unit_profile_t : public profile_t {

    private:

    public:

        versioned_string file_name;

        change_entity_map<decl_stmt_profile_t> decl_stmts;
        change_entity_map<function_profile_t>  functions;
        change_entity_map<class_profile_t>     classes;
        change_entity_map<profile_t>           conditionals;

    public:

        unit_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            file_name = name;
            
            if(!file_name.has_modified())      operation = SRCDIFF_DELETE;
            else if(!file_name.has_original()) operation = SRCDIFF_INSERT;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_decl_stmt(type_name))           decl_stmts.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(type_name))  functions.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(type_name))     classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, profile);
            else child_profiles.push_back(profile->id);
            
        }

        virtual std::ostream & summary(std::ostream & out) const {

            pad(out) << type_name << " '" << file_name << "': Impact = " << get_impact_factor() << '\n'; 
            // out << " Whitespace: " << whitespace_count;
            // out << "\tComment: " << comment_count;
            // out << "\tSyntax: " << syntax_count;
            // out << "\tTotal: " << total_count;
            // out << '\n';

            //++depth;

            decl_stmts.summarize_pure(out, SRCDIFF_DELETE);
            decl_stmts.summarize_pure(out, SRCDIFF_INSERT);
            decl_stmts.summarize_modified(out);

            functions.summarize_pure(out, SRCDIFF_DELETE);
            functions.summarize_pure(out, SRCDIFF_INSERT);
            functions.summarize_modified(out);

            classes.summarize_pure(out, SRCDIFF_DELETE);
            classes.summarize_pure(out, SRCDIFF_INSERT);
            classes.summarize_modified(out);

            conditionals.summarize_modified(out);

            size_t num_conditionals_deleted  = conditionals.count(SRCDIFF_DELETE);
            if(num_conditionals_deleted) pad(out) << "Number conditionals deleted: " << num_conditionals_deleted << '\n';

            size_t num_conditionals_inserted = conditionals.count(SRCDIFF_INSERT);
            if(num_conditionals_inserted) pad(out) << "Number conditionals inserted: " << num_conditionals_inserted << '\n';            size_t num_conditionals_modified = 0;

            std::for_each(conditionals.find(SRCDIFF_COMMON), conditionals.upper_bound(SRCDIFF_COMMON),
                [&num_conditionals_modified](const change_entity_map<profile_t>::pair & pair) { if(pair.second->syntax_count) ++num_conditionals_modified; });
            if(num_conditionals_modified) pad(out) << "Number conditionals modified: " << num_conditionals_modified << '\n';

            //--depth;

            return out;

        }

};

#endif
