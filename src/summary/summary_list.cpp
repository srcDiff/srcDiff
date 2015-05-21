#include <summary_list.hpp>

#include <conditional_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <call_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <if_profile_t.hpp>
#include <call_profile_t.hpp>
#include <expr_profile_t.hpp>
#include <ternary_profile_t.hpp>
#include <identifier_profile_t.hpp>
#include <identifier_diff.hpp>

#include <identifier_summary_t.hpp>
#include <replacement_summary_t.hpp>
#include <move_summary_t.hpp>
#include <interchange_summary_t.hpp>
#include <jump_summary_t.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <set>
#include <typeinfo>
#include <cstring>

/** @todo check asserts */

#define run_expr_statistics(PROFILE)                                                                                                \
std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists; \
std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;                                              \
size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;                                  \
bool identifier_rename_only = true;                                                                                                 \
std::set<std::reference_wrapper<const versioned_string>> identifier_renames;                                                        \
expr_statistics(PROFILE, identifier_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,     \
                deleted_other, inserted_other, modified_other,                                                                      \
                number_arguments_deleted, number_arguments_inserted, number_arguments_modified,                                     \
                identifier_rename_only, identifier_renames);                                                                        \

std::string summary_list::get_type_string(const std::shared_ptr<profile_t> & profile) const {

    if(is_if(profile->type_name)) {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);
        if(if_profile->is_guard()) return "guard clause";

    }

    if(profile->type_name == "else") return "else";

    if(profile->type_name == "elseif") return "else if";

    if(is_decl_stmt(profile->type_name)) return "declaration";

    if(is_expr_stmt(profile->type_name)) {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);
        if(expr_stmt_profile->assignment()) return "assignment";
        if(expr_stmt_profile->is_delete())  return "delete";
        if(expr_stmt_profile->call()) {
            std::vector<std::shared_ptr<call_profile_t>>::size_type number_calls = expr_stmt_profile->get_call_profiles().size();
            if(number_calls == 1)           return "call";
            else                            return "call chain";
        }
        return "expression";

    }

    if(is_comment(profile->type_name)) return profile->type_name;

    return profile->type_name;

}

void summary_list::identifiers(const std::map<identifier_diff, size_t> & identifiers) {


    for(std::map<identifier_diff, size_t>::const_iterator itr = identifiers.begin(); itr != identifiers.end(); ++itr) {

        summaries.emplace_back(identifier_summary_t(summary_t::IDENTIFIER, SRCDIFF_COMMON, itr->first.get_diff(), itr->first.complex()));

        std::map<identifier_diff, size_t>::iterator itersect_itr = output_identifiers.find(itr->first);
        if(itersect_itr == output_identifiers.end())
            output_identifiers.insert(itersect_itr, *itr);
        else
            itersect_itr->second += itr->second;

    }

}

void summary_list::replacement(const std::shared_ptr<profile_t> & profile, size_t & pos) {

    const std::shared_ptr<profile_t> & start_profile = profile->child_profiles[pos];

    std::vector<const std::shared_ptr<expr_stmt_profile_t>>   expr_stmt_deleted,    expr_stmt_inserted;
    std::vector<const std::shared_ptr<decl_stmt_profile_t>>   decl_stmt_deleted,    decl_stmt_inserted;
    std::vector<const std::shared_ptr<conditional_profile_t>> conditionals_deleted, conditionals_inserted;
    std::vector<const std::shared_ptr<profile_t>>             jump_deleted,         jump_inserted;
    std::vector<const std::shared_ptr<profile_t>>             comment_deleted,      comment_inserted;
    for(; pos < profile->child_profiles.size() && profile->child_profiles[pos]->is_replacement; ++pos) {

        const std::shared_ptr<profile_t> & replacement_profile = profile->child_profiles[pos];                    

        if(is_condition_type(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE)
                conditionals_deleted.push_back(reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(replacement_profile));
            else
                conditionals_inserted.push_back(reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(replacement_profile));

        } else if(is_jump(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE)
                jump_deleted.push_back(replacement_profile);
            else
                jump_inserted.push_back(replacement_profile);

        } else if(is_expr_stmt(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE)
                expr_stmt_deleted.push_back(reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(replacement_profile));
            else
                expr_stmt_inserted.push_back(reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(replacement_profile));

        } else if(is_decl_stmt(replacement_profile->type_name)){

            if(replacement_profile->operation == SRCDIFF_DELETE)
                decl_stmt_deleted.push_back(reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(replacement_profile));
            else
                decl_stmt_inserted.push_back(reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(replacement_profile));

        } else if(is_comment(replacement_profile->type_name)) {

            if(replacement_profile->operation == SRCDIFF_DELETE) comment_deleted.push_back(replacement_profile);
            else                                                 comment_inserted.push_back(replacement_profile);

        }

    }

    --pos;

    size_t number_deleted_types  = 0;
    if(expr_stmt_deleted.size() != 0)    ++number_deleted_types;
    if(decl_stmt_deleted.size() != 0)    ++number_deleted_types;
    if(conditionals_deleted.size() != 0) ++number_deleted_types;
    if(jump_deleted.size() != 0)         ++number_deleted_types;
    if(comment_deleted.size() != 0)      ++number_deleted_types;

    size_t number_syntax_deletions = expr_stmt_deleted.size() + decl_stmt_deleted.size() + conditionals_deleted.size() + jump_deleted.size();

    size_t number_inserted_types = 0;
    if(expr_stmt_inserted.size() != 0)    ++number_inserted_types;
    if(decl_stmt_inserted.size() != 0)    ++number_inserted_types;
    if(conditionals_inserted.size() != 0) ++number_inserted_types;
    if(jump_inserted.size() != 0)         ++number_inserted_types;
    if(comment_inserted.size() != 0)      ++number_inserted_types;

    size_t number_syntax_insertions = expr_stmt_inserted.size() + decl_stmt_inserted.size() + conditionals_inserted.size() + jump_inserted.size();

    if(((number_syntax_deletions == 1 && number_syntax_insertions == 0) || (number_syntax_insertions == 1 && number_syntax_deletions == 0))
        && (comment_deleted.size() >= 1 || comment_inserted.size() >= 1)) {

        std::shared_ptr<profile_t> single_profile;

        if(expr_stmt_deleted.size())
            single_profile = expr_stmt_deleted.back();
        else if(expr_stmt_inserted.size())
            single_profile = expr_stmt_inserted.back();
        else if(decl_stmt_deleted.size())
            single_profile = decl_stmt_deleted.back();
        else if(decl_stmt_inserted.size())
            single_profile = decl_stmt_inserted.back();
        else if(conditionals_deleted.size())
            single_profile = conditionals_deleted.back();
        else if(conditionals_inserted.size())
            single_profile = conditionals_inserted.back();
        else if(jump_deleted.size())
            single_profile = jump_deleted.back();
        else if(jump_inserted.size())
            single_profile = jump_inserted.back();

        if(number_syntax_deletions == 1)
            summaries.emplace_back(replacement_summary_t(summary_t::REPLACEMENT, SRCDIFF_COMMON,
                                                         1, get_type_string(single_profile), comment_deleted.size(), 0, std::string(), comment_inserted.size()));
        else
            summaries.emplace_back(replacement_summary_t(summary_t::REPLACEMENT, SRCDIFF_COMMON,
                                                         0, std::string(), comment_deleted.size(), 1, get_type_string(single_profile), comment_inserted.size()));

        return;

    }

    size_t number_original = 0;
    std::string original_type;

    if(number_syntax_deletions == 1) {

        number_original = 1;
        if(expr_stmt_deleted.size())
            original_type = get_type_string(expr_stmt_deleted.back());
        else if(decl_stmt_deleted.size())
            original_type = get_type_string(decl_stmt_deleted.back());
        else if(conditionals_deleted.size())
            original_type = get_type_string(conditionals_deleted.back());
        else if(jump_deleted.size())
            original_type = get_type_string(jump_deleted.back());
        else
            original_type = get_type_string(comment_deleted.back());

    } else {

        if(number_deleted_types == 1 || (comment_deleted.size() != 0 && number_deleted_types == 2)) {

            if(expr_stmt_deleted.size()) {

                number_original = expr_stmt_deleted.size();
                original_type = get_type_string(expr_stmt_deleted.back());

            } else if(decl_stmt_deleted.size()) {

                number_original = decl_stmt_deleted.size();
                original_type = get_type_string(decl_stmt_deleted.back());

            } else if(conditionals_deleted.size()) {

                number_original = conditionals_deleted.size();
                original_type = "conditional";

            } else if(jump_deleted.size()) {

                if(jump_deleted.size() == 1) {

                    number_original = 1;
                    original_type = get_type_string(jump_deleted.back());

                } else {

                    number_original = jump_deleted.size();
                    std::set<std::string> jump_types;
                    for(const std::shared_ptr<profile_t> & profile_ptr : jump_deleted)
                        jump_types.insert(profile_ptr->type_name.original());

                    if(jump_types.size() == 1)
                        original_type = get_type_string(jump_deleted.back());
                    else
                        original_type = "statement";

                }


            }

        } else {

            number_original = number_syntax_deletions;
            original_type = "statement";

        }

    }

    size_t number_modified = 0;
    std::string modified_type;

    if(number_syntax_insertions == 1) {

        number_modified = 1;
        if(expr_stmt_inserted.size())
            modified_type = get_type_string(expr_stmt_inserted.back());
        else if(decl_stmt_inserted.size())
            modified_type = get_type_string(decl_stmt_inserted.back());
        else if(conditionals_inserted.size())
            modified_type = get_type_string(conditionals_inserted.back());
        else if(jump_inserted.size())
            modified_type = get_type_string(jump_inserted.back());
        else
            modified_type = get_type_string(comment_inserted.back());

    } else {

        if(number_inserted_types == 1 || (comment_inserted.size() != 0 && number_inserted_types == 2)) {

            if(expr_stmt_inserted.size()) {

                number_modified = expr_stmt_inserted.size();
                modified_type = get_type_string(expr_stmt_inserted.back());

            } else if(decl_stmt_inserted.size()) {

                number_modified = decl_stmt_inserted.size();
                modified_type = get_type_string(decl_stmt_inserted.back());

            } else if(conditionals_inserted.size()) {

                number_modified = conditionals_inserted.size();
                modified_type = "conditional";

            } else if(jump_inserted.size()) {

                if(jump_inserted.size() == 1) {

                    number_modified = 1;
                    modified_type = get_type_string(jump_inserted.back());

                } else {

                    number_modified = jump_inserted.size();
                    std::set<std::string> jump_types;
                    for(const std::shared_ptr<profile_t> & profile_ptr : jump_inserted)
                        jump_types.insert(profile_ptr->type_name.modified());

                    if(jump_types.size() == 1)
                        modified_type = get_type_string(jump_inserted.back());
                    else
                        modified_type = "statement";

                }


            }

        } else {

            number_modified = number_syntax_insertions;
            modified_type = "statement";

        }

    }

    summaries.emplace_back(replacement_summary_t(summary_t::REPLACEMENT, SRCDIFF_COMMON,
                                                 number_original, original_type, comment_deleted.size(), number_modified, modified_type, comment_inserted.size()));

}

bool summary_list::is_body_summary(const std::string & type, bool is_replacement) const {

    return is_condition_type(type) || is_expr_stmt(type) || is_decl_stmt(type) || (is_comment(type) && is_replacement)
        || is_jump(type) || type == "else";

}

void summary_list::statement_dispatch(const std::shared_ptr<profile_t> & profile, size_t & child_pos) {

    const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[child_pos];

    if(child_profile->is_replacement && ((child_pos + 1) < profile->child_profiles.size())) {

        replacement(profile, child_pos);

    } else if(child_profile->move_id) {

        summaries.emplace_back(move_summary_t(summary_t::MOVE, SRCDIFF_COMMON, get_type_string(child_profile)));

    } else if(!child_profile->type_name.is_common()) {

        interchange(child_profile);

    } else {

        if(is_jump(child_profile->type_name))
            jump(child_profile);
        else if(is_condition_type(child_profile->type_name))
            conditional(child_profile);
        else if(child_profile->type_name == "else")
            else_clause(child_profile);
        else if(is_expr_stmt(child_profile->type_name))
            expr_stmt(child_profile);
        else if(is_decl_stmt(child_profile->type_name))
            decl_stmt(child_profile);

    }

}

size_t summary_list::number_child_changes(const profile_t::profile_list_t & child_profiles) const {

    size_t num_child_changes = 0;
    size_t num_body_changes  = 0;

    for(size_t child_pos = 0; child_pos < child_profiles.size(); ++child_pos) {

        const std::shared_ptr<profile_t> & child_profile = child_profiles[child_pos];

        if(child_profile->is_replacement && ((child_pos + 1) < child_profiles.size())) {

            for(; child_pos < child_profiles.size() && child_profiles[child_pos]->is_replacement; ++child_pos)
                ;
            --child_pos;

            ++num_child_changes;

        } else if(child_profile->move_id) {

            ++num_child_changes;

        } else if(!child_profile->type_name.is_common()) {

            ++num_child_changes;

        } else {

            if(is_jump(child_profile->type_name))
                ++num_child_changes;
            else if(is_condition_type(child_profile->type_name))
                ++num_body_changes;
            else if(child_profile->type_name == "else")
                ++num_body_changes;
            else if(is_expr_stmt(child_profile->type_name))
                ++num_child_changes;
            else if(is_decl_stmt(child_profile->type_name))
                ++num_child_changes;

        }

    }

    return num_child_changes > 0 ? num_child_changes + num_body_changes : num_child_changes;

}

summary_list::summary_list() {}

bool summary_list::identifier_check(const std::shared_ptr<profile_t> & profile, const std::map<identifier_diff, size_t> & identifier_set,
                                    std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    bool is_identifier_only = true;
    for(const std::shared_ptr<profile_t> & child_profile : profile->child_profiles) {

        if(is_identifier(child_profile->type_name)) {

            const std::shared_ptr<identifier_profile_t> & identifier_profile
                = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(child_profile);

            identifier_diff ident_diff(identifier_profile->name);
            ident_diff.trim(false);

            if(identifier_set.count(ident_diff))
                identifier_renames.insert(identifier_profile->name);


        } else if(child_profile->operation != SRCDIFF_COMMON) {

            is_identifier_only = false;

        } else {

            is_identifier_only = is_identifier_only && identifier_check(child_profile, identifier_set, identifier_renames);

        }

    }

    return is_identifier_only;

}

void summary_list::ternary(const std::shared_ptr<profile_t> & profile, const std::map<identifier_diff, size_t> & identifier_set,
                           bool & condition_modified, bool & then_clause_modified, bool & else_clause_modified,
                           std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    assert(typeid(*profile.get()) == typeid(ternary_profile_t));

    const std::shared_ptr<ternary_profile_t> & ternary_profile = reinterpret_cast<const std::shared_ptr<ternary_profile_t> &>(profile);

    if(ternary_profile->condition() && ternary_profile->condition()->syntax_count != 0) {

        /** @todo first case expression deleted and inserted.  Could possibly still be a rename if only a signle identifier may need to handle  */
        if(ternary_profile->condition()->child_profiles.size() != 1) {

            condition_modified = true;

        } else {

            run_expr_statistics(ternary_profile->condition()->child_profiles[0]);

            if(!identifier_rename_only) condition_modified = true;

        }

    }

    if(ternary_profile->then_clause() && ternary_profile->then_clause()->syntax_count != 0) {

        if(ternary_profile->then_clause()->child_profiles.size() != 1) {

            then_clause_modified = true;

        } else {

            run_expr_statistics(ternary_profile->then_clause()->child_profiles[0]);

            if(!identifier_rename_only) then_clause_modified = true;

        }

    }

    if(ternary_profile->else_clause() && ternary_profile->else_clause()->syntax_count != 0) {

        if(ternary_profile->else_clause()->child_profiles.size() != 1) {

            else_clause_modified = true;

        } else {

            run_expr_statistics(ternary_profile->else_clause()->child_profiles[0]);

            if(!identifier_rename_only) else_clause_modified = true;

        }

    }

}

void summary_list::expr_statistics(const std::shared_ptr<profile_t> & profile, const std::map<identifier_diff, size_t> & identifier_set,
                              std::vector<std::shared_ptr<call_profile_t>> & deleted_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & inserted_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & modified_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & renamed_calls,
                              std::vector<std::shared_ptr<call_profile_t>> & modified_argument_lists,
                              std::vector<std::shared_ptr<profile_t>> & deleted_other,
                              std::vector<std::shared_ptr<profile_t>> & inserted_other,
                              std::vector<std::shared_ptr<profile_t>> & modified_other,
                              size_t & number_arguments_deleted,
                              size_t & number_arguments_inserted,
                              size_t & number_arguments_modified,
                              bool & identifier_rename_only,
                              std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    assert(typeid(*profile.get()) == typeid(expr_profile_t));

    for(size_t pos = 0; pos < profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        if(child_profile->operation == SRCDIFF_COMMON && child_profile->syntax_count == 0) continue;

        if(child_profile->type_name.is_common() && is_call(child_profile->type_name)) {

            const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_profile);

            if(child_profile->operation == SRCDIFF_DELETE) {

                deleted_calls.push_back(call_profile);
                identifier_rename_only = false;

            } else if(child_profile->operation == SRCDIFF_INSERT) {

                inserted_calls.push_back(call_profile);
                identifier_rename_only = false;

            } else if(child_profile->operation == SRCDIFF_COMMON) {

                bool report_name = !call_profile->name.is_common();
                if(report_name) {

                    identifier_diff ident_diff(call_profile->name);
                    ident_diff.trim(true);

                    if(!identifier_set.count(ident_diff)) {
                        report_name = false;
                    }

                    identifier_rename_only = false;

                }

                bool report_argument_list = call_profile->argument_list_modified;
                if(report_argument_list) {

                    size_t number_deleted  = call_profile->arguments.count(SRCDIFF_DELETE);
                    size_t number_inserted = call_profile->arguments.count(SRCDIFF_INSERT);

                    number_arguments_deleted  += number_deleted;
                    number_arguments_inserted += number_inserted;

                    if(number_deleted != 0 || number_inserted != 0)
                        identifier_rename_only = false;

                    bool report_change = false;
                    std::for_each(call_profile->arguments.lower_bound(SRCDIFF_COMMON), call_profile->arguments.upper_bound(SRCDIFF_COMMON),
                        [&, this](const typename change_entity_map<profile_t>::pair & pair) {

                            if(pair.second->syntax_count == 0) return;

                            for(const std::shared_ptr<profile_t> & argument_child_profile : pair.second->child_profiles[0]->child_profiles) {

                               if(argument_child_profile->type_name.is_common() && is_call(argument_child_profile->type_name)) {

                                    std::vector<std::shared_ptr<call_profile_t>> inner_deleted_calls, inner_inserted_calls,
                                        inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists;
                                    std::vector<std::shared_ptr<profile_t>> inner_deleted_other, inner_inserted_other, inner_modified_other;
                                    size_t inner_number_arguments_deleted = 0, inner_number_arguments_inserted = 0, inner_number_arguments_modified = 0;
                                    size_t save_identifier_count = identifier_renames.size();
                                    expr_statistics(argument_child_profile->parent, identifier_set,
                                                    inner_deleted_calls, inner_inserted_calls, inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists,
                                                    inner_deleted_other, inner_inserted_other, inner_modified_other,
                                                    inner_number_arguments_deleted, inner_number_arguments_inserted, inner_number_arguments_modified,
                                                    identifier_rename_only, identifier_renames);

                                    if(inner_deleted_calls.size() != 0 || inner_inserted_calls.size() != 0 || inner_modified_calls.size() != 0
                                    || inner_deleted_other.size() != 0 || inner_inserted_other.size() != 0 || inner_modified_other.size() != 0) {

                                        report_change = true;
                                        identifier_rename_only = false;

                                    }

                                    if(save_identifier_count != identifier_renames.size()) {

                                        report_change = true;

                                    }

                                } else if(argument_child_profile->operation != SRCDIFF_COMMON) { 

                                    report_change = true;
                                    identifier_rename_only = false;

                                } else if(!is_identifier(argument_child_profile->type_name)) {

                                    report_change = true;
                                    identifier_rename_only = false;

                                } else {

                                    const std::shared_ptr<identifier_profile_t> & identifier_profile
                                        = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(argument_child_profile);

                                    identifier_diff ident_diff(identifier_profile->name);
                                    ident_diff.trim(false);

                                    if(identifier_set.count(ident_diff)) {

                                        identifier_renames.insert(identifier_profile->name);
                                        report_change = true;

                                    }

                                }

                            }

                            if(report_change)
                               ++number_arguments_modified;

                        });

                    if(number_deleted == 0 && number_inserted == 0 && !report_change)
                        report_argument_list = false;

                }

                if(!report_name && !report_argument_list) continue;

                modified_calls.push_back(call_profile);
                if(report_name)          renamed_calls.push_back(call_profile);
                if(report_argument_list) modified_argument_lists.push_back(call_profile);

            }

        } else {

            if(child_profile->operation != SRCDIFF_COMMON) {

                if(child_profile->operation == SRCDIFF_DELETE)
                    deleted_other.push_back(child_profile);
                else
                    inserted_other.push_back(child_profile);

                identifier_rename_only = false;

            } else {

                /** @todo need to handle things lambda and specialized calls.  Maybe more... */

                if(is_ternary(child_profile->type_name)) {

                    bool condition_modified = false, then_clause_modified = false, else_clause_modified = false;
                    ternary(child_profile, identifier_set, condition_modified, then_clause_modified, else_clause_modified, identifier_renames);
                    if(condition_modified || then_clause_modified || else_clause_modified) {

                        modified_other.push_back(child_profile);
                        identifier_rename_only = false;

                    }

                } else if(!is_identifier(child_profile->type_name)) {

                    size_t save_identifier_count = identifier_renames.size();
                    bool is_identifier_only = identifier_check(child_profile, identifier_set, identifier_renames);

                    if(!is_identifier_only || save_identifier_count != identifier_renames.size()) {

                        modified_other.push_back(child_profile);
                        identifier_rename_only = identifier_rename_only && is_identifier_only;

                    }

                } else {

                    const std::shared_ptr<identifier_profile_t> & identifier_profile
                        = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(child_profile);

                    identifier_diff ident_diff(identifier_profile->name);
                    ident_diff.trim(false);

                    if(identifier_set.count(ident_diff))
                        identifier_renames.insert(identifier_profile->name);


                }

            }

        }

     }

}

void summary_list::common_expr_stmt(const std::shared_ptr<profile_t> & profile) const {

    assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    std::map<identifier_diff, size_t> identifier_set;
    std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
                        output_identifiers.begin(), output_identifiers.end(),
                        std::inserter(identifier_set, identifier_set.begin()));

    run_expr_statistics(profile->child_profiles[0]);

    if(deleted_calls.size() == 0 && inserted_calls.size() == 0 && modified_calls.size() == 0
    && deleted_other.size() == 0 && inserted_other.size() == 0 && modified_other.size() == 0
    && identifier_renames.size() == 0) return;

    if(expr_stmt_profile->call())
        return call_sequence(profile, renamed_calls.size(), number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
                             modified_argument_lists.size(), identifier_rename_only, identifier_renames);

    size_t number_change_types = 0;
    if(deleted_calls.size() != 0)           ++number_change_types;
    if(inserted_calls.size() != 0)          ++number_change_types;
    if(renamed_calls.size() != 0)           ++number_change_types;
    if(modified_argument_lists.size() != 0) ++number_change_types;
    if(deleted_other.size() != 0)           ++number_change_types;
    if(inserted_other.size() != 0)          ++number_change_types;
    if(modified_other.size() != 0)          ++number_change_types;

}

static bool operator<(const std::__1::reference_wrapper<const versioned_string> & ref_one, const std::__1::reference_wrapper<const versioned_string> & ref_two) {

    return ref_one.get() < ref_two.get();

}

void summary_list::call_sequence(const std::shared_ptr<profile_t> & profile, size_t number_rename,
                                                    size_t number_arguments_deleted, size_t number_arguments_inserted, size_t number_arguments_modified,
                                                    size_t number_argument_lists_modified,
                                                    bool identifier_rename_only, const std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    std::vector<std::shared_ptr<call_profile_t>>::size_type calls_sequence_length = expr_stmt_profile->get_call_profiles().size();

    size_t number_arguments_total = number_arguments_deleted + number_arguments_inserted + number_arguments_modified;
    bool is_variable_reference_change = number_argument_lists_modified == 0;

    if(is_variable_reference_change) {

        for(std::vector<std::shared_ptr<call_profile_t>>::size_type pos = 0; pos < calls_sequence_length; ++pos) {

            const std::shared_ptr<call_profile_t> & call_profile = expr_stmt_profile->get_call_profiles()[pos];

            if((call_profile->operation == SRCDIFF_COMMON && call_profile->argument_list_modified)
                || (call_profile->operation != SRCDIFF_COMMON && pos != (calls_sequence_length - 1))) {

                is_variable_reference_change = false;
                break;

            }

       }

    }

}

void summary_list::expr_stmt(const std::shared_ptr<profile_t> & profile) const {

    assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    if((expr_stmt_profile->assignment() && expr_stmt_profile->operation != SRCDIFF_COMMON) || expr_stmt_profile->is_delete() || profile->child_profiles.empty()) {


    }

    if(profile->operation == SRCDIFF_COMMON) {

        common_expr_stmt(profile);

    }

}

/** @todo for decl_stmt and jump need to not report if only a known rename identifier occurs.  Also, report a rename if it occurred */
/** @todo report type rename and name rename.  Report as type and name change probably. */
void summary_list::decl_stmt(const std::shared_ptr<profile_t> & profile) const {

    assert(typeid(*profile.get()) == typeid(decl_stmt_profile_t));

    const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);

    const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    std::map<identifier_diff, size_t> identifier_set;
    std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
                        output_identifiers.begin(), output_identifiers.end(),
                        std::inserter(identifier_set, identifier_set.begin()));

    size_t number_parts_report = 0;
    bool identifier_rename_only = true;
    std::set<std::reference_wrapper<const versioned_string>> identifier_renames;
    if(decl_stmt_profile->operation == SRCDIFF_COMMON) {

        if(!decl_stmt_profile->type.is_common()) {

            identifier_diff ident_diff(decl_stmt_profile->type);
            ident_diff.trim(true);

            if(identifier_set.count(ident_diff))
                ++number_parts_report;

        }

        if(!decl_stmt_profile->name.is_common()) {

            identifier_diff ident_diff(decl_stmt_profile->name);
            ident_diff.trim(true);

            if(identifier_set.count(ident_diff))
                ++number_parts_report;

        }

        if(!decl_stmt_profile->init.is_common()) {

            run_expr_statistics(decl_stmt_profile->child_profiles.back());

            /** @todo need to probably output if single identifier change */
            if(deleted_calls.size() != 0 || inserted_calls.size() != 0 || modified_calls.size() != 0
            || deleted_other.size() != 0 || inserted_other.size() != 0 || modified_other.size() != 0
            || identifier_renames.size() != 0)
                ++number_parts_report;

        }

        if(number_parts_report == 0) return;

    }

}

void summary_list::else_clause(const std::shared_ptr<profile_t> & profile) {

    if(!profile->type_name.is_common())
        return interchange(profile);

    assert(profile->type_name == "else");

    const bool has_common = profile->common_profiles.size() > 0;

    const bool output_else = profile->operation != SRCDIFF_COMMON || number_child_changes(profile->child_profiles) > 1;

    if(profile->parent->operation != SRCDIFF_COMMON) {



    }

    if(profile->summary_identifiers.size() > 0) {

        identifiers(profile->summary_identifiers);

    }

    for(size_t pos = 0; pos < profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        if((child_profile->syntax_count > 0 || child_profile->move_id
            || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
            && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            statement_dispatch(profile, pos);

        }

    }

}

/** @todo if multiple of same change like test case where connect deleted 4 times.  May want to summarize in one line. */
void summary_list::conditional(const std::shared_ptr<profile_t> & profile) {

    assert(is_condition_type(profile->type_name));

    const bool has_common = profile->common_profiles.size() > 0;

    const std::shared_ptr<conditional_profile_t> & conditional_profile = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile);

    const bool condition_modified = conditional_profile->is_condition_modified();
    const bool body_modified = conditional_profile->is_body_modified();

    boost::optional<srcdiff_type> else_operation;
    if(profile->type_name == "if") else_operation = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->else_operation();
    const bool else_modified = bool(else_operation) && *else_operation == SRCDIFF_COMMON;

    boost::optional<srcdiff_type> elseif_operation;
    if(profile->type_name == "if") elseif_operation = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->elseif_operation();;
    const bool elseif_modified = bool(elseif_operation) && *elseif_operation == SRCDIFF_COMMON;

    const versioned_string & condition = conditional_profile->get_condition();

    if(!condition_modified && !body_modified && bool(else_operation)
        && (profile->operation == SRCDIFF_COMMON || profile->child_profiles.back()->common_profiles.size() > 0))
        return else_clause(profile->child_profiles[0]);

    const std::shared_ptr<profile_t> & summary_profile = profile->type_name == "elseif" && profile->child_profiles.size() == 1
        && profile->child_profiles[0]->type_name == "if" ? profile->child_profiles[0] : profile;

    const bool output_conditional = summary_profile->operation != SRCDIFF_COMMON || condition_modified || number_child_changes(summary_profile->child_profiles) > 1;

    size_t statement_count = summary_profile->operation == SRCDIFF_DELETE ? summary_profile->statement_count_original : summary_profile->statement_count_modified;
    if(profile->type_name == "elseif") --statement_count;
    const size_t common_statements = summary_profile->common_statements;

    if(condition_modified) {

         

    }

    if(summary_profile->summary_identifiers.size() > 0) {

        identifiers(summary_profile->summary_identifiers);

    }

    for(size_t pos = 0; pos < summary_profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = summary_profile->child_profiles[pos];

        if((child_profile->syntax_count > 0 || child_profile->move_id
            || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
            && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            statement_dispatch(summary_profile, pos);

        }

    }

}

void summary_list::interchange(const std::shared_ptr<profile_t> & profile) {

    assert(!profile->type_name.is_common());

    summaries.emplace_back(interchange_summary_t(summary_t::INTERCHANGE, SRCDIFF_COMMON,
            versioned_string(profile->type_name.original() == "elseif" ? "else if" : profile->type_name.original(),
                             profile->type_name.modified() == "elseif" ? "else if" : profile->type_name.modified())));

    std::shared_ptr<profile_t> summary_profile = profile;
    if(profile->type_name.original() == "elseif" || profile->type_name.modified() == "elseif")
        summary_profile = profile->child_profiles[0];

    for(size_t pos = 0; pos < summary_profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        if((child_profile->syntax_count > 0 || child_profile->move_id
            || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
            && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            statement_dispatch(profile, pos);

        }

    }

}


void summary_list::jump(const std::shared_ptr<profile_t> & profile) {

    assert(is_jump(profile->type_name));

    const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    std::map<identifier_diff, size_t> identifier_set;
    std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
                        output_identifiers.begin(), output_identifiers.end(),
                        std::inserter(identifier_set, identifier_set.begin()));

    if(profile->operation == SRCDIFF_COMMON) {

        run_expr_statistics(profile->child_profiles.back());

        /** @todo need to probably output if single identifier change */
        if(deleted_calls.size() == 0 && inserted_calls.size() == 0 && modified_calls.size() == 0
        && deleted_other.size() == 0 && inserted_other.size() == 0 && modified_other.size() == 0
        && identifier_renames.size() == 0)
            return;

    }

    summaries.emplace_back(jump_summary_t(summary_t::JUMP, profile->operation, get_type_string(profile)));

}

void summary_list::body(const profile_t & profile) {

    identifiers(profile.summary_identifiers);

    for(size_t pos = 0; pos < profile.child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile.child_profiles[pos];

        if(!is_body_summary(child_profile->type_name, child_profile->is_replacement)
            || (child_profile->operation == SRCDIFF_COMMON && child_profile->syntax_count == 0
                && child_profile->move_id == 0))
            continue;

        statement_dispatch(std::make_shared<profile_t>(profile), pos);

    }

}
