#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>

std::string get_article(const std::shared_ptr<profile_t> & profile) const { 

    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
    if(is_guard_clause) return "a";

    const char letter = std::string(profile->type_name)[0];

    if(letter == 'a' || letter == 'i' || letter == 'o' || letter == 'u')
        return "an";
    else
        return "a";
}

std::ostream & summary_visitor(std::ostream & out, const std::shared_ptr<profile_t> & profile, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
    const bool has_common = profile->has_common;

    begin_line(out);

    // before children
    if(profile->operation == SRCDIFF_COMMON) out << "the body of ";

    out << get_article(profile) << ' ';

    if(is_guard_clause) out << "guard clause was ";
    else                out << profile->type_name << " statement was ";


    if(profile->operation != SRCDIFF_COMMON)
         out << (profile->operation == SRCDIFF_DELETE ? "removed" : "added");
    else out << "modified";

    if(profile->operation != SRCDIFF_COMMON && has_common) out << " retaining the statement body";

    bool is_leaf = true;
    for(size_t child_pos : profile->child_profiles) {

        const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

        /** @todo check this condition */
        if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
             && is_condition_type(child_profile->type_name)) {

            if(is_leaf) {

                out << " this includes:\n";
                is_leaf = false;

            }

            ++depth;
            summary_visitor(out, child_profile, profile_list);
            --depth;

        }

    }

    // after children
    if(is_leaf) out << '\n';

    return out;

}

std::ostream & conditional_text_summary(std::ostream & out, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

    /**
    	Probably should be on child profiles and summarize top down, nesting when needed.

    	If only one deep print out added/deleted to function.

    	Else print out modified and report modifications recursively.
    */
    for(size_t profile_pos : child_profiles) {

        const std::shared_ptr<profile_t> & profile = profile_list[profile_pos];

        if(!is_condition_type(profile->type_name) || (profile->operation == SRCDIFF_COMMON && profile->syntax_count == 0))
            continue;

        if(false && profile->parent_id == id) {

		    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;

	        begin_line(out);

        	/** ? modified ? */
            if(profile->operation == SRCDIFF_COMMON) continue;

            out << get_article(profile) << ' ';

            if(is_guard_clause) out << "guard clause was ";
            else                out << profile->type_name << " statement was ";

            out << (profile->operation == SRCDIFF_DELETE ? "removed from " : (has_common ? "added " : "added to "));

            if(has_common) out << " around existing code in ";

            out << "the function body\n";

        } else {

            summary_visitor(out, profile, profile_list);

        }

    }

    return out;

}

#endif