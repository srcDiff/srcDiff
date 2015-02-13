#ifndef INCLUDED_UNIT_PROFILE_T_HPP
#define INCLUDED_UNIT_PROFILE_T_HPP

#include <profile_t.hpp>
#include <function_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <versioned_string.hpp>

struct unit_profile_t : public profile_t {

    private:

    public:

        versioned_string file_name;

        std::multimap<srcdiff_type, std::shared_ptr<decl_stmt_profile_t>> decl_stmts;
        std::multimap<srcdiff_type, std::shared_ptr<function_profile_t>> functions;

        unit_profile_t(std::string type_name, srcdiff_type operation) : profile_t(type_name, operation) {}

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {

            file_name = name;
            
            if(!file_name.has_modified())      operation = SRCDIFF_DELETE;
            else if(!file_name.has_original()) operation = SRCDIFF_INSERT;

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            if(is_function_type(profile->type_name))  functions.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<function_profile_t> &>(profile));
            else if(is_decl_stmt(profile->type_name)) decl_stmts.emplace(profile->operation, reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile));
            else child_profiles.push_back(profile->id);
            
        }

        template<typename T>
        static std::ostream & summarize_pure(std::ostream & out, const std::multimap<srcdiff_type, std::shared_ptr<T>> & map, srcdiff_type operation) {

            size_t count = map.count(operation);
            if(count == 0) return out;

            out << '\n';

           typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator citr = map.find(operation);

            out << (operation == SRCDIFF_DELETE ? "Deleted " : "Inserted ") << citr->second->type_name << "s (" << count << "): { ";
            citr->second->summary(out);
            ++citr;
            for(; citr != map.upper_bound(operation); ++citr) {

                out << ", ";
                citr->second->summary(out);

            }

            out << " }\n";

            return out;

        }

        template<typename T>
        static std::ostream & summarize_modified(std::ostream & out, const std::multimap<srcdiff_type, std::shared_ptr<T>> & map) {

            size_t num_modified = map.count(SRCDIFF_COMMON);
            if(num_modified == 0) return out;

            out << '\n';

            typename std::multimap<srcdiff_type, std::shared_ptr<T>>::const_iterator citr = map.find(SRCDIFF_COMMON);

            out << "Modified " << citr->second->type_name << "s: " << num_modified << '\n';
            for(; citr != map.upper_bound(SRCDIFF_COMMON); ++citr)
                citr->second->summary(out);

            return out;

        }

        virtual std::ostream & summary(std::ostream & out) const {

            out << type_name << " '" << file_name << "':"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            summarize_pure<decl_stmt_profile_t>(out, decl_stmts, SRCDIFF_DELETE);
            summarize_pure<decl_stmt_profile_t>(out, decl_stmts, SRCDIFF_INSERT);
            summarize_modified<decl_stmt_profile_t>(out, decl_stmts);

            summarize_pure<function_profile_t>(out, functions, SRCDIFF_DELETE);
            summarize_pure<function_profile_t>(out, functions, SRCDIFF_INSERT);
            summarize_modified<function_profile_t>(out, functions);

            return out;


        }

};

#endif