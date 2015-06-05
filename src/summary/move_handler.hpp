#ifndef INCLUDED_MOVE_HANDLER_HPP
#define INCLUDED_MOVE_HANDLER_HPP

#include <profile_t.hpp>
#include <versioned_string.hpp>

#include <vector>
#include <string>

#include <cctype>

class move_handler {

private:

    size_t & statement_count;
    size_t & statement_churn;

	profile_t::profile_list_t move_candidates;

    bool compare_ignore_whitespace(const std::string & one, const std::string & two) {

        std::string::size_type pos_one = 0, pos_two = 0;
        while(pos_one < one.size() && pos_two < two.size()) {

            if(isspace(one[pos_one]) || isspace(two[pos_two])) {

                if(isspace(one[pos_one]))
                    ++pos_one;

                if(isspace(two[pos_two]))
                    ++pos_two;            

                continue;

            }

            if(one[pos_one] != two[pos_two]) return false;

            ++pos_one, ++pos_two;


        }

        while(pos_one < one.size() && isspace(one[pos_one]))
            ++pos_one;

        while(pos_two < two.size() && isspace(two[pos_two]))
            ++pos_two;

        return pos_one == one.size() && pos_two == two.size();

    }

	void set_strings(srcdiff_type first_operation, const versioned_string & first, const versioned_string & second, versioned_string & original, versioned_string & modified) const {

        original = first_operation == SRCDIFF_DELETE ? first  : second;
        modified = first_operation == SRCDIFF_DELETE ? second : first;		

	}

	void set_move(std::shared_ptr<profile_t> & first_profile, std::shared_ptr<profile_t> & second_profile) {

		if(!first_profile->raw.empty() && compare_ignore_whitespace(first_profile->raw, second_profile->raw)) {

            --statement_count;
            statement_churn -= 2;

			first_profile->move_id = (size_t)-1;
        	first_profile->move_parent = second_profile->parent;
            for(profile_t::profile_list_t::iterator itr = second_profile->parent->child_change_profiles.begin(); itr != second_profile->parent->child_change_profiles.end(); ++itr) {

                if(second_profile == *itr) {


                    second_profile->parent->child_change_profiles.erase(itr);
                    break;
                }

            }
		}

	}

public:

	move_handler(size_t & statement_count, size_t & statement_churn)
        : statement_count(statement_count), statement_churn(statement_churn) {}

    virtual void gather_candidates(const profile_t::profile_list_t & descendant_change_profiles) {

        for(const std::shared_ptr<profile_t> & descendant_change_profile : descendant_change_profiles) {

            if(is_statement(descendant_change_profile->type_name) && descendant_change_profile->operation != SRCDIFF_COMMON)
                move_candidates.push_back(descendant_change_profile);

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
