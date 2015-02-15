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

    public:

        unit_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            file_name = name;
            
            if(!file_name.has_modified())      operation = SRCDIFF_DELETE;
            else if(!file_name.has_original()) operation = SRCDIFF_INSERT;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            if(is_decl_stmt(profile->type_name))          decl_stmts.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else if(is_function_type(profile->type_name)) functions.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_class_type(profile->type_name))    classes.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<class_profile_t> &>(profile));
            else child_profiles.push_back(profile->id);
            
        }

        virtual impact_factor calculate_impact_factor() const {

            impact_factor max_factor = NONE;
            std::for_each(functions.find(SRCDIFF_COMMON), functions.upper_bound(SRCDIFF_COMMON),
                          [&max_factor](const change_entity_map<function_profile_t>::pair & profile_pair) { 
                                        if(max_factor == HIGH) return;
                                        impact_factor factor = profile_pair.second->calculate_impact_factor();
                                        if(factor > max_factor) max_factor = factor; });

            std::for_each(classes.find(SRCDIFF_COMMON), classes.upper_bound(SRCDIFF_COMMON),
              [&max_factor](const change_entity_map<class_profile_t>::pair & profile_pair) { 
                            if(max_factor == HIGH) return;
                            impact_factor factor = profile_pair.second->calculate_impact_factor();
                            if(factor > max_factor) max_factor = factor; });

            impact_factor factor = default_calculate_impact_factor();

            return factor > max_factor ? factor : max_factor;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            pad(out) << type_name << " '" << file_name << "': Impact = " << get_impact_factor() << '\n'; 
            // out << " Whitespace: " << whitespace_count;
            // out << "\tComment: " << comment_count;
            // out << "\tSyntax: " << syntax_count;
            // out << "\tTotal: " << total_count;
            out << '\n';

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

            //--depth;

            return out;

        }

};

#endif