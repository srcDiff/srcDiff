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

	void set_move(std::shared_ptr<profile_t> & profile, const std::pair<versioned_string, versioned_string> & first_pair, const std::pair<versioned_string, versioned_string> & second_pair) {

		if(first_pair.first.has_original() == first_pair.second.has_modified()
                && first_pair.first.original() == first_pair.second.modified()
                && second_pair.first.has_original() == second_pair.second.has_modified()
                && second_pair.first.original() == second_pair.second.modified())
			profile->move_id = (size_t)-1;

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

                if(first_profile->type_name == "expr_stmt") {

                    std::shared_ptr<expr_stmt_profile_t> & first_expr_stmt_profile  = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(first_profile);
                    std::shared_ptr<expr_stmt_profile_t> & second_expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(second_profile);

                    versioned_string original_lhs, modified_lhs;
                    set_strings(first_profile->operation, first_expr_stmt_profile->lhs(), second_expr_stmt_profile->lhs(), original_lhs, modified_lhs);
                    versioned_string original_rhs, modified_rhs;
                    set_strings(first_profile->operation, first_expr_stmt_profile->rhs(), second_expr_stmt_profile->rhs(), original_rhs, modified_rhs);

                    set_move(first_profile, std::make_pair(original_lhs, modified_lhs), std::make_pair(original_rhs, modified_rhs));

                } else if(first_profile->type_name == "decl_stmt") {

                    std::shared_ptr<decl_stmt_profile_t> & first_decl_stmt_profile  = reinterpret_cast<std::shared_ptr<decl_stmt_profile_t> &>(first_profile);
                    std::shared_ptr<decl_stmt_profile_t> & second_decl_stmt_profile = reinterpret_cast<std::shared_ptr<decl_stmt_profile_t> &>(second_profile);

                    versioned_string original_type, modified_type;
                    set_strings(first_profile->operation, first_decl_stmt_profile->type, second_decl_stmt_profile->type, original_type, modified_type);
                    versioned_string original_name, modified_name;
                    set_strings(first_profile->operation, first_decl_stmt_profile->name, second_decl_stmt_profile->name, original_name, modified_name);

                    set_move(first_profile, std::make_pair(original_type, modified_type), std::make_pair(original_name, modified_name));

                }

                if(first_profile->move_id) {

                    for(profile_t::profile_list_t::iterator itr = second_profile->parent->child_profiles.begin(); itr != second_profile->parent->child_profiles.end(); ++itr) {

                        if(second_profile == *itr) {


                            second_profile->parent->child_profiles.erase(itr);
                            break;
                        }

                    }

                }

            }

        }

    }

};

#endif