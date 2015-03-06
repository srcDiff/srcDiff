#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <memory>

std::string get_article(const std::shared_ptr<profile_t> & profile) const  { 

    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
    if(is_guard_clause) return "a";

    const char letter = std::string(profile->type_name)[0];

    if(letter == 'a' || letter == 'i' || letter == 'o' || letter == 'u')
        return "an";
    else
        return "a";
}

std::ostream & summary_visitor(std::ostream & out, const std::shared_ptr<profile_t> & profile, const std::vector<std::shared_ptr<profile_t>> & profile_list) const {

    // before children 

    bool is_leaf = true;
    for(size_t child_pos : profile->child_profiles) {

        const std::shared_ptr<profile_t> & child_profile = profile_list[child_pos];

        /** @todo check this condition */
        if((child_profile->syntax_count > 0 || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
             && is_condition_type(child_profile->type_name)) {

            is_leaf = false;
            summary_visitor(out, child_profile, profile_list);

        }

    }

    if(is_leaf)
        ;

    // after children

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

        if(profile->parent_id == id) {

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

			// size_t current_id = profile->id;
			// size_t parent_id = profile->parent_id;
			// size_t parent_level = 0;

			// while(parent_id != id) {

	  //      		const std::shared_ptr<profile_t> & current_profile = profile_list[current_id];
	  //      		const std::shared_ptr<profile_t> & parent_profile = profile_list[parent_id];
		 //        const bool is_guard_clause = current_profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(current_profile)->is_guard() : false;
		 //        const bool is_parent_guard_clause = parent_profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(parent_profile)->is_guard() : false;

	  //           const bool has_common = current_profile->has_common;

		 //        begin_line(out);

	  //       	if(current_profile->operation == SRCDIFF_COMMON) out << "the body of ";

	  //       	if(parent_level == 0)
		 //        	out << get_article(current_profile) << ' ';
		 //        else {

		 //        	out << "the same ";

		 //        }

	  //           if(is_guard_clause) out << "guard clause was ";
	  //           else                out << current_profile->type_name << " statement was ";

	  //           if(current_profile->operation != SRCDIFF_COMMON) {

		 //            out << (current_profile->operation == SRCDIFF_DELETE ? "removed from " : "added to ");

	  //           } else {

		 //            out << "modified from within ";

			// 	}

	  //      		out << "the body of ";

	  //      		if(current_profile->type_name == parent_profile->type_name)
	  //      			out << "another ";
	  //      		else
	  //     			out << get_article(parent_profile) << ' ';

	  //           if(is_parent_guard_clause) out << "guard clause ";
		 //        else                       out << parent_profile->type_name << " statement ";

		 //        out << '\n';

		 //        current_id = parent_id;
		 //        parent_id = parent_profile->parent_id;
		 //        ++parent_level;

	  //   	}

        }

    }

    return out;

}

#endif