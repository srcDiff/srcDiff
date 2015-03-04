#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>

std::ostream & conditional_text_summary(std::ostream & out, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

    /** recursively look throught children to find leaf profiles, no children or no modified children */
    std::vector<size_t> summary_profiles;
    for(size_t profile_pos : child_profiles) {

       std::vector<size_t> child_visits;
       child_visits.push_back(profile_pos);
        while(!child_visits.empty()) {

            size_t child = child_visits.back();
            child_visits.pop_back();
            const std::shared_ptr<profile_t> & profile = profile_list[child];

            bool is_leaf = true;
            for(size_t child_pos : profile->child_profiles) {

                const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

                /** @todo check this condition */
                if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
                     && is_condition_type(child_profile->type_name)) {

                    is_leaf = false;
                    child_visits.push_back(child_pos);

                }

            }

            if(is_leaf) summary_profiles.push_back(child);

        }

    }

        /** todo so if parent is deleted/inserted then should report as part of base or new document or say context */
    std::function<std::string (const std::shared_ptr<profile_t> & profile)> get_article 
        = [](const std::shared_ptr<profile_t> & profile) { 

            const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
            if(is_guard_clause) return "a";

            const char letter = std::string(profile->type_name)[0];

            if(letter == 'a' || letter == 'i' || letter == 'o' || letter == 'u')
                return "an";
            else
                return "a";
        };

    for(size_t profile_pos : summary_profiles) {

        const std::shared_ptr<profile_t> & profile = profile_list[profile_pos];

        if(!is_condition_type(profile->type_name) || (profile->operation == SRCDIFF_COMMON && profile->syntax_count == 0))
            continue;

        const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
        const bool has_common = profile->has_common;

        begin_line(out);

        if(profile->parent_id == id) {

            if(profile->operation == SRCDIFF_COMMON) continue;

            out << get_article(profile) << ' ';

            if(is_guard_clause) out << "guard clause was ";
            else                out << profile->type_name << " statement was ";

            out << (profile->operation == SRCDIFF_DELETE ? "removed from " : (has_common ? "added " : "added to "));

            if(has_common) out << " around existing code in ";

            out << "the function body\n";

        } else {

            const std::shared_ptr<profile_t> & parent_profile = profile_list[profile->parent_id];
            const bool is_parent_guard_clause = parent_profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(parent_profile)->is_guard() : false;
            const bool has_common = profile->has_common;

            if(profile->operation != SRCDIFF_COMMON) {

            	out << get_article(profile) << ' ';

                if(is_guard_clause) out << "guard clause was ";
                else                out << profile->type_name << " statement was ";

                out << (profile->operation == SRCDIFF_DELETE ? "removed from " : "added to ");

                out << "the body of " << get_article(parent_profile) << ' ';

                if(is_parent_guard_clause) out << "guard clause";
                else                       out << parent_profile->type_name << " statement";

                out << '\n';

            } else {

            	out << "the body of " << get_article(profile) << ' ';

                if(is_guard_clause) out << "guard clause within ";
                else                out << profile->type_name << " statement within ";

                if(parent_profile->operation != SRCDIFF_COMMON) out << (parent_profile->operation == SRCDIFF_DELETE ? "the deleted " : "the inserted ");
                else if(!is_parent_guard_clause) out << get_article(parent_profile) << ' ';

                if(is_parent_guard_clause) out << " guard clause ";
                else                       out << parent_profile->type_name << " statement ";

                out << "was modified\n";

            }

        }

    }

    return out;

}

#endif