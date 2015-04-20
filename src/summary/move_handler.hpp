#ifndef INCLUDED_MOVE_HANDLER_HPP
#define INCLUDED_MOVE_HANDLER_HPP

#include <profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <versioned_string.hpp>

#include <vector>
#include <string>

class move_handler {

private:

	profile_t::profile_list_t move_candidates;

	void set_strings(srcdiff_type first_operation, const versioned_string & first, const versioned_string & second, versioned_string & original, versioned_string & modified) const {

        original = first_operation == SRCDIFF_DELETE ? first  : second;
        modified = first_operation == SRCDIFF_DELETE ? second : first;		

	}

	void set_move(std::shared_ptr<profile_t> & first_profile, std::shared_ptr<profile_t> & second_profile) {

		if(!first_profile->raw.empty() && first_profile->raw == second_profile->raw) {

			first_profile->move_id = (size_t)-1;
        	first_profile->move_parent = second_profile->parent;
            for(profile_t::profile_list_t::iterator itr = second_profile->parent->child_profiles.begin(); itr != second_profile->parent->child_profiles.end(); ++itr) {

                if(second_profile == *itr) {


                    second_profile->parent->child_profiles.erase(itr);
                    break;
                }

            }
		}

	}

public:

	move_handler() {}

    virtual void gather_candidates(const profile_t::profile_list_t & descendant_profiles) {

        for(const std::shared_ptr<profile_t> & descendant_profile : descendant_profiles) {

            if(is_statement(descendant_profile->type_name) && descendant_profile->operation != SRCDIFF_COMMON)
                move_candidates.push_back(descendant_profile);

        }

    }

    virtual void detect() {

        for(size_t first = 0; first < move_candidates.size(); ++first) {

            std::shared_ptr<profile_t> & first_profile = move_candidates[first];

            for(size_t second = first + 1; second < move_candidates.size(); ++second) {

                std::shared_ptr<profile_t> & second_profile = move_candidates[second];
                if(first_profile->operation == second_profile->operation) continue;
                if(first_profile->type_name != second_profile->type_name) continue;

                set_move(first_profile, second_profile);

            }

        }

    }

};

#endif