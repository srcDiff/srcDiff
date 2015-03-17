#ifndef INCLUDED_UNIT_PROFILE_T_HPP
#define INCLUDED_UNIT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <function_profile_t.hpp>
#include <class_profile_t.hpp>
#include <versioned_string.hpp>
#include <change_entity_map.hpp>
#include <type_query.hpp>
#include <table_summary.hpp>

class unit_profile_t : public profile_t {

    private:

    public:

        versioned_string file_name;

        change_entity_map<decl_stmt_profile_t>   decl_stmts;
        change_entity_map<function_profile_t>    functions;
        change_entity_map<class_profile_t>       classes;
        change_entity_map<conditional_profile_t> conditionals;

    public:

        unit_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) : profile_t(type_name, uri, operation, parent_id) {}

        virtual void set_name(versioned_string name, const boost::optional<versioned_string> & parent) {

            file_name = name;
            
            if(!file_name.has_modified())      operation = SRCDIFF_DELETE;
            else if(!file_name.has_original()) operation = SRCDIFF_INSERT;

        }

        virtual void add_descendant(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_decl_stmt(type_name))           decl_stmts.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(type_name))  functions.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(type_name))     classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile));
            
            descendant_profiles.insert(std::lower_bound(descendant_profiles.begin(), descendant_profiles.end(), profile->id), profile->id);
            
        }

        virtual std::ostream & summary(std::ostream & out, size_t summary_types, const profile_list_t & profile_list) const {

            begin_line(out) << "file '" << file_name << "': Impact = " << get_impact_factor() << '\n'; 

            /** might want to have table summaries.  Decl may need to be changed how output */
            decl_stmts.summarize_pure(out, summary_types, profile_list, SRCDIFF_DELETE);
            decl_stmts.summarize_pure(out, summary_types, profile_list, SRCDIFF_INSERT);
            decl_stmts.summarize_modified(out, summary_types, profile_list);

            functions.summarize_pure(out, summary_types, profile_list, SRCDIFF_DELETE);
            functions.summarize_pure(out, summary_types, profile_list, SRCDIFF_INSERT);
            functions.summarize_modified(out, summary_types, profile_list);

            classes.summarize_pure(out, summary_types, profile_list, SRCDIFF_DELETE);
            classes.summarize_pure(out, summary_types, profile_list, SRCDIFF_INSERT);
            classes.summarize_modified(out, summary_types, profile_list);

            if(!is_summary_type(summary_types, summary_type::TABLE)) return out;

            table_summary table(conditionals);

            size_t number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified = 0;
            conditionals.count_operations(number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);
            if(number_conditionals_deleted || number_conditionals_inserted || number_conditionals_modified)
                table.output_all_conditional_counts(out, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);

            return out;

        }

};

#endif
