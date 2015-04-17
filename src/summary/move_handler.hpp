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

                    versioned_string original_lhs = first_expr_stmt_profile->operation == SRCDIFF_DELETE ? first_expr_stmt_profile->lhs()  : second_expr_stmt_profile->lhs();
                    versioned_string modified_lhs = first_expr_stmt_profile->operation == SRCDIFF_DELETE ? second_expr_stmt_profile->lhs() : first_expr_stmt_profile->lhs();

                    versioned_string original_rhs = first_expr_stmt_profile->operation == SRCDIFF_DELETE ? first_expr_stmt_profile->rhs()  : second_expr_stmt_profile->rhs();
                    versioned_string modified_rhs = first_expr_stmt_profile->operation == SRCDIFF_DELETE ? second_expr_stmt_profile->rhs() : first_expr_stmt_profile->rhs();


                    if(original_lhs.has_original() == modified_lhs.has_modified()
                        && original_lhs.original() == modified_lhs.modified()
                        && original_rhs.has_original() == modified_rhs.has_modified()
                        && original_rhs.original() == modified_rhs.modified())
                        first_profile->move_id = (size_t)-1;

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