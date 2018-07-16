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

        /** 
         * @todo this assumes declarations are simple names.  Believe some class at least can be complex, and function definitions are complex 
         * and need to be resolved into a simple
         */
        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> identifier_to_declaration_profile;

    public:

        unit_profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation) : profile_t(type_name, uri, operation) {}

        virtual void add_descendant_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent) {

            const std::string type_name = profile->type_name.is_common() ? std::string(profile->type_name) : profile->type_name.original();

            if(is_decl_stmt(type_name))           decl_stmts.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(type_name))  functions.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(type_name))     classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));
            else if(is_condition_type(type_name)) conditionals.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile));
            
            descendant_change_profiles.insert(std::lower_bound(descendant_change_profiles.begin(), descendant_change_profiles.end(), profile), profile);
            
        }

        const versioned_string & get_name() const {
            return file_name;
        }

        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types) const {

            out.begin_line() << "file '" << file_name << "': Impact = " << get_impact_factor() << '\n'; 

            /** might want to have table summaries.  Decl may need to be changed how output */
            // decl_stmts.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            // decl_stmts.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            // decl_stmts.summarize_modified(out, summary_types);

            functions.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            functions.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            functions.summarize_modified(out, summary_types);

            // classes.summarize_pure(out, summary_types, SRCDIFF_DELETE);
            // classes.summarize_pure(out, summary_types, SRCDIFF_INSERT);
            classes.summarize_modified(out, summary_types);

            if(!is_summary_type(summary_types, summary_type::TABLE)) return out;

            table_summary table(conditionals);

            size_t number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified = 0;
            conditionals.count_operations(number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);
            if(number_conditionals_deleted || number_conditionals_inserted || number_conditionals_modified) {
                table.output_all_conditional_counts(out, number_conditionals_deleted, number_conditionals_inserted, number_conditionals_modified);
            }

            return out;

        }

};

#endif
