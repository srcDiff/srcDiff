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
#include <exception_profile_t.hpp>
#include <interchange_profile_t.hpp>

#include <identifier_utilities.hpp>

#include <identifier_summary_t.hpp>
#include <replace_summary_t.hpp>
#include <move_summary_t.hpp>
#include <interchange_summary_t.hpp>
#include <jump_summary_t.hpp>
#include <conditional_summary_t.hpp>
#include <expr_stmt_summary_t.hpp>
#include <call_sequence_summary_t.hpp>
#include <expr_stmt_calls_summary_t.hpp>
#include <decl_stmt_summary_t.hpp>
#include <exception_summary_t.hpp>
#include <label_summary_t.hpp>
#include <macro_summary_t.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <set>
#include <typeinfo>
#include <cstring>

/** @todo check asserts */

#define run_expr_statistics(PROFILE)                                                                                                    \
    std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists; \
    std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;                                              \
    size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;                                  \
    bool identifier_rename_only = true;                                                                                                 \
    std::set<std::reference_wrapper<const versioned_string>> identifier_renames;                                                        \
    expr_statistics(PROFILE, identifier_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,     \
                    deleted_other, inserted_other, modified_other,                                                                      \
                    number_arguments_deleted, number_arguments_inserted, number_arguments_modified,                                     \
                    identifier_rename_only, identifier_renames);

#define is_expr_syntax_change()                                                              \
      (deleted_calls.size() != 0 || inserted_calls.size() != 0 || modified_calls.size() != 0 \
    || deleted_other.size() != 0 || inserted_other.size() != 0 || modified_other.size() != 0 \
    || identifier_renames.size() != 0)

#define no_expr_syntax_change()                                                              \
      (deleted_calls.size() == 0 && inserted_calls.size() == 0 && modified_calls.size() == 0 \
    && deleted_other.size() == 0 && inserted_other.size() == 0 && modified_other.size() == 0 \
    && identifier_renames.size() == 0)

static bool is_candidate_name_change(const versioned_string & identifier, const std::multiset<versioned_string> & uses, size_t number_threshold) {

    if(identifier.is_common()) return false;

     if(identifier.has_original() && identifier.has_modified() && uses.size() > number_threshold) return true;

     // should always have an original or modified so no need to check as well
     if(!identifier.has_original() || !identifier.has_modified()) {

        size_t number_changed = 0;
        for(const versioned_string & v_string : uses) {

            if(v_string.is_common()) continue;

            if(!v_string.has_original() || !v_string.has_modified()) continue;

            ++number_changed;

        }

        if(number_changed > number_threshold) return true;

     }

     return false;

}

bool compare_identifier_map(const std::pair<versioned_string, std::multiset<versioned_string>> & first,
                            const std::pair<versioned_string, std::multiset<versioned_string>> & second) {

    return first.first < second.first;

}

#define identifier_set_difference(PROFILE)                                                                               \
    std::map<versioned_string, std::multiset<versioned_string>> identifier_set;                                          \
    std::map<versioned_string, std::multiset<versioned_string>> identifier_list;                                         \
    for(const std::pair<std::string, std::map<versioned_string,                                                          \
        std::multiset<versioned_string>>> & identifier_map : PROFILE->identifiers)                                       \
            for(const std::pair<versioned_string, std::multiset<versioned_string>> & identifier : identifier_map.second) \
                if(is_candidate_name_change(identifier.first, identifier.second, 0))                                     \
                    identifier_list[identifier.first] = identifier.second;                                               \
    std::set_difference(identifier_list.begin(), identifier_list.end(),                                                  \
                        output_identifiers.begin(), output_identifiers.end(),                                            \
                        std::inserter(identifier_set, identifier_set.begin()), compare_identifier_map);

static inline bool is_name_change(const std::shared_ptr<identifier_profile_t> & identifier_profile,
                                        const std::map<versioned_string, std::multiset<versioned_string>> & identifier_set) {

    bool is_found = true;
    for(const versioned_string & v_string : identifier_profile->simple_names) {

        if(v_string.is_common()) continue;

        std::map<versioned_string, std::multiset<versioned_string>>::const_iterator citr = identifier_set.find(v_string);
        if(citr != identifier_set.end() && citr->second.find(identifier_profile->name) != citr->second.end()) continue;

        is_found = false;
        break;

    }

    return is_found;

}

std::string summary_list::get_type_string(const std::shared_ptr<profile_t> & profile) const {

    if(is_if(profile->type_name)) {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);
        if(if_profile->is_guard()) return "guard-clause";
        if(if_profile->else_clause() && if_profile->operation != SRCDIFF_COMMON)
            return "if-else";
    }

    if(profile->type_name == "else") return "else";

    if(profile->type_name == "elseif") return "else-if";

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
        if(expr_stmt_profile->print())      return "print";
        return "expression";

    }

    if(is_exception_handling(profile->type_name)) {

        const std::shared_ptr<exception_profile_t> & exception_profile = reinterpret_cast<const std::shared_ptr<exception_profile_t> &>(profile);
        if(profile->type_name == "try" && profile->operation != SRCDIFF_COMMON && exception_profile->catches() > 0)
            return "try-catch";

    }

    if(is_comment(profile->type_name)) return profile->type_name;

    return profile->type_name;

}

bool summary_list::is_block_summary(const std::string & type, bool is_replace) const {

    return is_condition_type(type) || is_expr_stmt(type) || is_decl_stmt(type) || (is_comment(type) && is_replace)
        || is_jump(type) || type == "else" || is_exception_handling(type) || is_label(type) || is_expr_block(type)
        || is_interchange(type) || type == "macro";

}

void summary_list::statement_dispatch(const std::shared_ptr<profile_t> & profile, size_t & child_pos) {

    const std::shared_ptr<profile_t> & child_change_profile = profile->child_change_profiles[child_pos];

    if(child_change_profile->is_replace && ((child_pos + 1) < profile->child_change_profiles.size())) {

        replace(profile, child_pos);

    } else if(child_change_profile->move_id) {

        summaries_.emplace_back(new move_summary_t(get_type_string(child_change_profile)));

    } else if(child_change_profile->type_name == "interchange") {

        interchange(child_change_profile);

    } else {

        if(is_jump(child_change_profile->type_name))
            jump(child_change_profile);
        else if(is_condition_type(child_change_profile->type_name))
            conditional(child_change_profile);
        else if(child_change_profile->type_name == "else")
            else_clause(child_change_profile);
        else if(is_expr_stmt(child_change_profile->type_name))
            expr_stmt(child_change_profile);
        else if(is_decl_stmt(child_change_profile->type_name))
            decl_stmt(child_change_profile);
        else if(is_exception_handling(child_change_profile->type_name))
            exception(child_change_profile);
        else if(is_label(child_change_profile->type_name))
            label(child_change_profile);
        else if(is_expr_block(child_change_profile->type_name))
            block(child_change_profile);
        else if(child_change_profile->type_name == "macro")
            macro(child_change_profile);


    }

}

void summary_list::block(const std::shared_ptr<profile_t> & profile) {

    for(size_t pos = 0; pos < profile->child_change_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_change_profile = profile->child_change_profiles[pos];

        if((child_change_profile->syntax_count > 0 || child_change_profile->move_id
            || (child_change_profile->operation != SRCDIFF_COMMON && (profile->operation != child_change_profile->operation || is_expr_block(profile->type_name))))
            && is_block_summary(child_change_profile->type_name.first_active_string(), child_change_profile->is_replace)) {

            statement_dispatch(profile, pos);

        }

    }

}

void summary_list::identifiers(std::map<std::string, std::set<versioned_string>> profile_declarations,
                               std::map<std::string, std::map<versioned_string, std::multiset<versioned_string>>> profile_identifiers) {

    std::vector<versioned_string> rename_identifiers;
    for(std::map<std::string, std::set<versioned_string>>::iterator itr = profile_declarations.begin(); itr != profile_declarations.end();) {

        if(itr->second.size() == 1 && !itr->second.begin()->is_common()
            && itr->second.begin()->has_original() && itr->second.begin()->has_modified()
            && profile_declarations[itr->second.begin()->original()].size() == 1
            && profile_declarations[itr->second.begin()->modified()].size() == 1) {

            versioned_string save_identifier = *itr->second.begin();
    
            while(itr != profile_declarations.end()
                &&    ((save_identifier.has_original() && save_identifier.original() == itr->first)
                    || (save_identifier.has_modified() && save_identifier.modified() == itr->first)))
                ++itr;
    
            if(save_identifier.has_original())
                profile_declarations.erase(save_identifier.original());

            if(save_identifier.has_modified())
                profile_declarations.erase(save_identifier.modified());

            rename_identifiers.push_back(save_identifier);

            continue;

        }

        ++itr;

    }

    for(std::vector<versioned_string>::const_iterator itr = rename_identifiers.begin(); itr != rename_identifiers.end(); ++itr) {

        summaries_.emplace_back(new identifier_summary_t(*itr, true));

        output_identifiers[*itr] = std::multiset<versioned_string>();

    }

    std::vector<std::pair<versioned_string, versioned_string>> name_change_identifiers;
    for(std::map<std::string, std::map<versioned_string, std::multiset<versioned_string>>>::iterator
        itr = profile_identifiers.begin(); itr != profile_identifiers.end(); ++itr) {

        for(std::map<versioned_string, std::multiset<versioned_string>>::iterator 
            use_itr = itr->second.begin(); use_itr != itr->second.end();) {

            if(is_candidate_name_change(use_itr->first, use_itr->second, 1)) {

                if(use_itr->first.has_original() && use_itr->first.has_modified()) {

                    name_change_identifiers.emplace_back(use_itr->first, use_itr->first);
                    ++use_itr;
                    profile_identifiers[name_change_identifiers.back().first.original()].erase(name_change_identifiers.back().first);
                    profile_identifiers[name_change_identifiers.back().first.modified()].erase(name_change_identifiers.back().first);

                } else {

                    std::multiset<versioned_string>::const_iterator citr = use_itr->second.begin();
                    for(; citr != use_itr->second.end(); ++citr)
                        if(!citr->is_common() && citr->has_original() && citr->has_modified())
                            break;

                    versioned_string extended_name_change = use_itr->first;
                    if(citr != use_itr->second.end()) {

                       identifier_utilities ident(*citr, false);

                        const std::vector<std::pair<std::string, srcdiff_type>> list = ident.list();

                        std::string token;
                        srcdiff_type operation;
                        if(use_itr->first.has_original()) {

                            token = use_itr->first.original();
                            operation = SRCDIFF_DELETE;

                        } else {

                            token = use_itr->first.modified();
                            operation = SRCDIFF_INSERT;

                        }

                        std::vector<std::pair<std::string, srcdiff_type>>::size_type pos = 0;
                        for(; pos < list.size(); ++pos) {

                            if(list[pos].second != operation) continue;
                            if(list[pos].first != token)      continue;

                            break;

                        }

                        std::vector<std::pair<std::string, srcdiff_type>>::size_type start_pos = pos;
                        if(start_pos > 0 && list[start_pos - 1].second == operation && !identifier_utilities::is_identifier_char(list[start_pos - 1].first[0]))
                            --start_pos;
                        if(start_pos > 0 && list[start_pos - 1].second == SRCDIFF_COMMON)
                            --start_pos;

                        std::vector<std::pair<std::string, srcdiff_type>>::size_type end_pos = pos;
                        if((end_pos + 1) < list.size() && list[end_pos + 1].second == operation && !identifier_utilities::is_identifier_char(list[end_pos + 1].first[0]))
                            ++end_pos;
                        if((end_pos + 1) < list.size() && list[end_pos + 1].second == SRCDIFF_COMMON)
                            ++end_pos;

                        extended_name_change = versioned_string();
                        for(std::vector<std::pair<std::string, srcdiff_type>>::size_type i = start_pos; i <= end_pos; ++i)
                            extended_name_change.append(list[i].first.c_str(), list[i].first.size(), list[i].second);

                    }

                    name_change_identifiers.emplace_back(use_itr->first, extended_name_change);
                    ++use_itr;

                    if(name_change_identifiers.back().first.has_original())
                        profile_identifiers[name_change_identifiers.back().first.original()].erase(name_change_identifiers.back().first);

                    if(name_change_identifiers.back().first.has_modified())
                        profile_identifiers[name_change_identifiers.back().first.modified()].erase(name_change_identifiers.back().first);

                }

            } else {

                ++use_itr;

            }

        }

    }

    /** @todo if a single rename probably need to not report as name change but entity modified.  Exceptions for some call name change etc. */
    for(std::vector<std::pair<versioned_string, versioned_string>>::const_iterator itr = name_change_identifiers.begin(); itr != name_change_identifiers.end(); ++itr) {

        if(output_identifiers.find(itr->first) != output_identifiers.end()) continue;

        output_identifiers[itr->first] = std::multiset<versioned_string>();

        if(output_identifiers_full.find(itr->second) != output_identifiers_full.end()) continue;

        output_identifiers_full.insert(itr->second);

        summaries_.emplace_back(new identifier_summary_t(itr->second, false));

    }

}

/** @todo this needs to be more generic so stuff like identifier can be replaced or anyother
    arbitrary item */
void summary_list::replace(const std::shared_ptr<profile_t> & profile, size_t & pos) {

    const std::shared_ptr<profile_t> & start_profile = profile->child_change_profiles[pos];

    /** @todo may want to optimze to enum */
    std::map<std::string, std::vector<const std::shared_ptr<profile_t>>> entity_deleted, entity_inserted;
    for(; pos < profile->child_change_profiles.size() && profile->child_change_profiles[pos]->is_replace; ++pos) {

        const std::shared_ptr<profile_t> & replace_profile = profile->child_change_profiles[pos];                    

        if(is_condition_type(replace_profile->type_name)) {

            if(replace_profile->operation == SRCDIFF_DELETE)
                entity_deleted["conditional"].push_back(replace_profile);
            else
                entity_inserted["conditional"].push_back(replace_profile);

        } else {

            if(replace_profile->operation == SRCDIFF_DELETE)
                entity_deleted[replace_profile->type_name].push_back(replace_profile);
            else
                entity_inserted[replace_profile->type_name].push_back(replace_profile);

        }

    }

    --pos;

    size_t number_deleted_types  = entity_deleted.size();
    bool is_comment_deleted = entity_deleted.find("comment") != entity_deleted.end();
    size_t number_syntax_deletions = 0;
    std::for_each(entity_deleted.begin(), entity_deleted.end(), [&number_syntax_deletions] (const std::pair<std::string, std::vector<const std::shared_ptr<profile_t>>> & entity) {

        if(entity.first != "comment")
            number_syntax_deletions += entity.second.size();

    });

    size_t number_inserted_types = entity_inserted.size();
    bool is_comment_inserted = entity_inserted.find("comment") != entity_inserted.end();
    size_t number_syntax_insertions = 0;
    std::for_each(entity_inserted.begin(), entity_inserted.end(), [&number_syntax_insertions] (const std::pair<std::string, std::vector<const std::shared_ptr<profile_t>>> & entity) {

        if(entity.first != "comment")
            number_syntax_insertions += entity.second.size();

    });

    if(((number_syntax_deletions == 1 && number_syntax_insertions == 0)
        || (number_syntax_insertions == 1 && number_syntax_deletions == 0))
            && (is_comment_deleted || is_comment_inserted)) {

        std::shared_ptr<profile_t> single_profile;
        if(number_syntax_deletions) {
            single_profile = entity_deleted.begin()->second.back();
        }
        else
            single_profile = entity_inserted.begin()->second.back();

        if(number_syntax_deletions == 1)
            summaries_.emplace_back(new replace_summary_t(1, get_type_string(single_profile), entity_deleted["comment"].size(), 0, std::string(), entity_inserted["comment"].size()));
        else
            summaries_.emplace_back(new replace_summary_t(0, std::string(), entity_deleted["comment"].size(), 1, get_type_string(single_profile), entity_inserted["comment"].size()));

        return;

    }

    size_t number_original = 0;
    std::string original_type;

    if(number_syntax_deletions == 1) {

        number_original = 1;
        original_type = get_type_string(entity_deleted.begin()->second.back());

    } else {

        if(number_deleted_types == 1 || (entity_deleted["comment"].size() != 0 && number_deleted_types == 2)) {

            std::map<std::string, std::vector<const std::shared_ptr<profile_t>>>::const_iterator entity
            = std::find_if_not(entity_deleted.begin(), entity_deleted.end(),
                [](const std::pair<std::string, std::vector<const std::shared_ptr<profile_t>>> & entity) { return entity.first == "comment"; });

            number_original = entity->second.size();
            original_type = get_type_string(entity->second.back());

        } else {

            number_original = number_syntax_deletions;
            original_type = "statement";

        }

    }

    size_t number_modified = 0;
    std::string modified_type;

    if(number_syntax_insertions == 1) {

        number_modified = 1;
        modified_type = get_type_string(entity_inserted.begin()->second.back());

    } else {

        if(number_inserted_types == 1 || (entity_inserted["comment"].size() != 0 && number_inserted_types == 2)) {

            std::map<std::string, std::vector<const std::shared_ptr<profile_t>>>::const_iterator entity
            = std::find_if_not(entity_inserted.begin(), entity_inserted.end(),
                [](const std::pair<std::string, std::vector<const std::shared_ptr<profile_t>>> & entity) { return entity.first == "comment"; });

            number_modified = entity->second.size();
            modified_type = get_type_string(entity->second.back());

        } else {

            number_modified = number_syntax_insertions;
            modified_type = "statement";

        }

    }

    summaries_.emplace_back(new replace_summary_t(number_original, original_type, entity_deleted["comment"].size(), number_modified, modified_type, entity_inserted["comment"].size()));

}

void summary_list::interchange(const std::shared_ptr<profile_t> & profile) {

    assert(profile->type_name == "interchange");

    const std::shared_ptr<interchange_profile_t> & interchange_profile = reinterpret_cast<const std::shared_ptr<interchange_profile_t> &>(profile);

    summaries_.emplace_back(new interchange_summary_t(versioned_string(get_type_string(interchange_profile->original()),
                                                                       get_type_string(interchange_profile->modified()))));

    if(!has_body(profile->type_name.original())) return;

    std::shared_ptr<profile_t> summary_profile = profile;
    if(profile->type_name.original() == "elseif" || profile->type_name.modified() == "elseif")
        summary_profile = profile->child_change_profiles[0];

    block(summary_profile);

}

void summary_list::jump(const std::shared_ptr<profile_t> & profile) {

    assert(is_jump(profile->type_name));

    const std::shared_ptr<profile_t> & body_profile = profile->body;
    identifier_set_difference(body_profile);

    if(profile->operation == SRCDIFF_COMMON) {

        run_expr_statistics(profile->child_change_profiles.back());

        /** @todo need to probably output if single identifier change */
        if(no_expr_syntax_change())
            return;

    }

    summaries_.emplace_back(new jump_summary_t(profile->operation, get_type_string(profile)));

}

void summary_list::else_clause(const std::shared_ptr<profile_t> & profile) {

    assert(profile->type_name == "else");

    if(profile->operation != SRCDIFF_COMMON)
        summaries_.emplace_back(new conditional_summary_t(profile->operation, get_type_string(profile), false));

    identifiers(profile->declarations, profile->identifiers);

    block(profile);

}

/** @todo if multiple of same change like test case where connect deleted 4 times.  May want to summarize in one line. */
void summary_list::conditional(const std::shared_ptr<profile_t> & profile) {

    assert(is_condition_type(profile->type_name));

    const std::shared_ptr<conditional_profile_t> & conditional_profile = reinterpret_cast<const std::shared_ptr<conditional_profile_t> &>(profile);

    const bool condition_modified = conditional_profile->is_condition_modified();
    const bool body_modified = conditional_profile->is_body_modified();

    bool is_internal = profile->operation != SRCDIFF_COMMON && profile->operation == profile->summary_parent->operation;

    const std::shared_ptr<profile_t> & summary_profile = profile->type_name == "elseif" && profile->child_change_profiles.size() == 1
        && profile->child_change_profiles[0]->type_name == "if" ? profile->child_change_profiles[0] : profile;

    if(condition_modified || (summary_profile->operation != SRCDIFF_COMMON && !is_internal))
        summaries_.emplace_back(new conditional_summary_t(summary_profile->operation, get_type_string(summary_profile), condition_modified));

    if(summary_profile->identifiers.size() > 0)
        identifiers(summary_profile->declarations, summary_profile->identifiers);

    block(summary_profile);

}

static bool operator<(const std::__1::reference_wrapper<const versioned_string> & ref_one, const std::__1::reference_wrapper<const versioned_string> & ref_two) {

    return ref_one.get() < ref_two.get();

}

bool summary_list::identifier_check(const std::shared_ptr<profile_t> & profile, const std::map<versioned_string, std::multiset<versioned_string>> & identifier_set,
                                    std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    bool is_identifier_only = profile->child_change_profiles.size() != 0;
    for(const std::shared_ptr<profile_t> & child_change_profile : profile->child_change_profiles) {

        if(is_identifier(child_change_profile->type_name)) {

            const std::shared_ptr<identifier_profile_t> & identifier_profile
                = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(child_change_profile);

            bool is_found = is_name_change(identifier_profile, identifier_set);

            if(is_found)
                identifier_renames.insert(identifier_profile->name);


        } else if(child_change_profile->operation != SRCDIFF_COMMON) {

            is_identifier_only = false;

        } else {

            is_identifier_only = is_identifier_only && identifier_check(child_change_profile, identifier_set, identifier_renames);

        }

    }

    return is_identifier_only;

}

void summary_list::ternary(const std::shared_ptr<profile_t> & profile, const std::map<versioned_string, std::multiset<versioned_string>> & identifier_set,
                           bool & condition_modified, bool & then_clause_modified, bool & else_clause_modified,
                           std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    assert(typeid(*profile.get()) == typeid(ternary_profile_t));

    const std::shared_ptr<ternary_profile_t> & ternary_profile = reinterpret_cast<const std::shared_ptr<ternary_profile_t> &>(profile);

    if(ternary_profile->condition() && ternary_profile->condition()->syntax_count != 0) {

        /** @todo first case expression deleted and inserted.  Could possibly still be a rename if only a signle identifier may need to handle  */
        if(ternary_profile->condition()->child_change_profiles.size() != 1) {

            condition_modified = true;

        } else {

            run_expr_statistics(ternary_profile->condition()->child_change_profiles[0]);

            if(!identifier_rename_only) condition_modified = true;

        }

    }

    if(ternary_profile->then_clause() && ternary_profile->then_clause()->syntax_count != 0) {

        if(ternary_profile->then_clause()->child_change_profiles.size() != 1) {

            then_clause_modified = true;

        } else {

            run_expr_statistics(ternary_profile->then_clause()->child_change_profiles[0]);

            if(!identifier_rename_only) then_clause_modified = true;

        }

    }

    if(ternary_profile->else_clause() && ternary_profile->else_clause()->syntax_count != 0) {

        if(ternary_profile->else_clause()->child_change_profiles.size() != 1) {

            else_clause_modified = true;

        } else {

            run_expr_statistics(ternary_profile->else_clause()->child_change_profiles[0]);

            if(!identifier_rename_only) else_clause_modified = true;

        }

    }

}

void summary_list::expr_statistics(const std::shared_ptr<profile_t> & profile, const std::map<versioned_string, std::multiset<versioned_string>> & identifier_set,
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

    for(size_t pos = 0; pos < profile->child_change_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_change_profile = profile->child_change_profiles[pos];

        if(child_change_profile->operation == SRCDIFF_COMMON && child_change_profile->syntax_count == 0) continue;

        if(child_change_profile->type_name.is_common() && is_call(child_change_profile->type_name)) {

            const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_change_profile);

            if(child_change_profile->operation == SRCDIFF_DELETE) {

                deleted_calls.push_back(call_profile);
                identifier_rename_only = false;

            } else if(child_change_profile->operation == SRCDIFF_INSERT) {

                inserted_calls.push_back(call_profile);
                identifier_rename_only = false;

            } else if(child_change_profile->operation == SRCDIFF_COMMON) {

                bool report_name = !call_profile->name->name.is_common();
                if(report_name) {

                    bool is_found = is_name_change(call_profile->name, identifier_set);

                    if(!is_found)
                        report_name = false;

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

                            for(const std::shared_ptr<profile_t> & argument_child_change_profile : pair.second->child_change_profiles[0]->child_change_profiles) {

                               if(argument_child_change_profile->type_name.is_common() && is_call(argument_child_change_profile->type_name)) {

                                    std::vector<std::shared_ptr<call_profile_t>> inner_deleted_calls, inner_inserted_calls,
                                        inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists;
                                    std::vector<std::shared_ptr<profile_t>> inner_deleted_other, inner_inserted_other, inner_modified_other;
                                    size_t inner_number_arguments_deleted = 0, inner_number_arguments_inserted = 0, inner_number_arguments_modified = 0;
                                    size_t save_identifier_count = identifier_renames.size();
                                    expr_statistics(argument_child_change_profile->summary_parent, identifier_set,
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

                                } else if(argument_child_change_profile->operation != SRCDIFF_COMMON) { 

                                    report_change = true;
                                    identifier_rename_only = false;

                                } else if(!is_identifier(argument_child_change_profile->type_name)) {

                                    report_change = true;
                                    identifier_rename_only = false;

                                } else {

                                    const std::shared_ptr<identifier_profile_t> & identifier_profile
                                        = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(argument_child_change_profile);

                                    bool is_found = is_name_change(identifier_profile, identifier_set);

                                    if(is_found) {

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

            if(child_change_profile->operation != SRCDIFF_COMMON) {

                if(child_change_profile->operation == SRCDIFF_DELETE)
                    deleted_other.push_back(child_change_profile);
                else
                    inserted_other.push_back(child_change_profile);

                identifier_rename_only = false;

            } else {

                /** @todo need to handle things lambda and specialized calls.  Maybe more... */

                if(is_ternary(child_change_profile->type_name)) {

                    bool condition_modified = false, then_clause_modified = false, else_clause_modified = false;
                    ternary(child_change_profile, identifier_set, condition_modified, then_clause_modified, else_clause_modified, identifier_renames);
                    if(condition_modified || then_clause_modified || else_clause_modified) {

                        modified_other.push_back(child_change_profile);
                        identifier_rename_only = false;

                    }

                } else if(!is_identifier(child_change_profile->type_name)) {

                    size_t save_identifier_count = identifier_renames.size();
                    bool is_identifier_only = identifier_check(child_change_profile, identifier_set, identifier_renames);

                    if(!is_identifier_only || save_identifier_count != identifier_renames.size()) {

                        modified_other.push_back(child_change_profile);
                        identifier_rename_only = identifier_rename_only && is_identifier_only;

                    }

                } else {

                    const std::shared_ptr<identifier_profile_t> & identifier_profile
                        = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(child_change_profile);

                    bool is_found = is_name_change(identifier_profile, identifier_set);

                    if(is_found)
                        identifier_renames.insert(identifier_profile->name);


                }

            }

        }

     }

}

void summary_list::common_expr_stmt(const std::shared_ptr<profile_t> & profile) {

    assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    const std::shared_ptr<profile_t> & body_profile = profile->body;

    identifier_set_difference(body_profile);

    run_expr_statistics(profile->child_change_profiles[0]);

    if(no_expr_syntax_change()) return;

    if(expr_stmt_profile->call())
        return call_sequence(profile, renamed_calls.size(), number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
                             modified_argument_lists.size(), identifier_rename_only, identifier_renames);

    size_t number_change_types = 0, number_call_types = 0, number_other_types = 0;
    if(deleted_calls.size() != 0)           ++number_change_types, ++number_call_types;
    if(inserted_calls.size() != 0)          ++number_change_types, ++number_call_types;
    if(renamed_calls.size() != 0)           ++number_change_types, ++number_call_types;
    if(modified_argument_lists.size() != 0) ++number_change_types, ++number_call_types;
    if(deleted_other.size() != 0)           ++number_change_types, ++number_other_types;
    if(inserted_other.size() != 0)          ++number_change_types, ++number_other_types;
    if(modified_other.size() != 0)          ++number_change_types, ++number_other_types;

    if(identifier_rename_only && identifier_renames.size() == 1) {

        summaries_.emplace_back(new identifier_summary_t(identifier_renames.begin()->get(), false));

    } else if(number_change_types == 1 && number_call_types == 1) {

        if(modified_argument_lists.size() <= 1)
            summaries_.emplace_back(new 
                expr_stmt_calls_summary_t(get_type_string(profile), deleted_calls.size(), inserted_calls.size(), renamed_calls.size(),
                                          modified_argument_lists.size(), number_arguments_deleted, number_arguments_inserted, number_arguments_modified));
        else
            summaries_.emplace_back(new expr_stmt_summary_t(profile->operation, get_type_string(profile)));

    } else {

       summaries_.emplace_back(new expr_stmt_summary_t(profile->operation, get_type_string(profile)));

    }

}

void summary_list::call_sequence(const std::shared_ptr<profile_t> & profile, size_t number_rename,
                                                    size_t number_arguments_deleted, size_t number_arguments_inserted, size_t number_arguments_modified,
                                                    size_t number_argument_lists_modified,
                                                    bool identifier_rename_only, const std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) {

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

    if(number_rename == 1 && identifier_renames.size() == 0 && number_argument_lists_modified == 0) {

        summaries_.emplace_back(new call_sequence_summary_t(get_type_string(profile), true, false));

    } else if(is_variable_reference_change) {

        summaries_.emplace_back(new call_sequence_summary_t(get_type_string(profile), false, true));

    } else if(identifier_rename_only && identifier_renames.size() == 1) {

        summaries_.emplace_back(new identifier_summary_t(identifier_renames.begin()->get(), false));

    } else if(number_argument_lists_modified == 1 && number_rename == 0) {

        summaries_.emplace_back(new 
                expr_stmt_calls_summary_t(get_type_string(profile), 0, 0, number_rename,
                                          number_argument_lists_modified, number_arguments_deleted, number_arguments_inserted, number_arguments_modified));

    } else {

        summaries_.emplace_back(new expr_stmt_summary_t(profile->operation, get_type_string(profile)));

    }

}

void summary_list::expr_stmt(const std::shared_ptr<profile_t> & profile) {

    assert(typeid(*profile.get()) == typeid(expr_stmt_profile_t));

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    if((expr_stmt_profile->assignment() && expr_stmt_profile->operation != SRCDIFF_COMMON)
        || expr_stmt_profile->is_delete() || expr_stmt_profile->print() || profile->child_change_profiles.empty()) {

        summaries_.emplace_back(new expr_stmt_summary_t(profile->operation, get_type_string(profile)));

    } else if(profile->operation == SRCDIFF_COMMON) {

        common_expr_stmt(profile);

    }

}

/** @todo for decl_stmt and jump need to not report if only a known rename identifier occurs.  Also, report a rename if it occurred */
/** @todo report type rename and name rename.  Report as type and name change probably. */
void summary_list::decl_stmt(const std::shared_ptr<profile_t> & profile) {

    assert(typeid(*profile.get()) == typeid(decl_stmt_profile_t));

    const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);

    const std::shared_ptr<profile_t> & body_profile = profile->body;

    identifier_set_difference(body_profile);

    size_t number_parts_report = 0;
    boost::optional<versioned_string> identifier_rename;
    if(decl_stmt_profile->operation == SRCDIFF_COMMON) {

        if(decl_stmt_profile->specifiers.size() != 0) {

            ++number_parts_report;

        }

        if(decl_stmt_profile->type->syntax_count > 0) {

            std::set<std::reference_wrapper<const versioned_string>> identifier_renames;
            bool is_identifier_only = identifier_check(decl_stmt_profile->type, identifier_set, identifier_renames);

            if(!is_identifier_only || identifier_renames.size() != 0)
                ++number_parts_report;

            if(is_identifier_only && identifier_renames.size() == 1)
                identifier_rename = identifier_renames.begin()->get();

        }

        if(!decl_stmt_profile->name.is_common()) {

            if(identifier_set.find(decl_stmt_profile->name) != identifier_set.end())
                ++number_parts_report;

        }

        if(decl_stmt_profile->init && decl_stmt_profile->init->syntax_count > 0) {

            if(decl_stmt_profile->init->child_change_profiles.size() != 1) {

                ++number_parts_report;

            } else {

                run_expr_statistics(decl_stmt_profile->init->child_change_profiles[0]);

                /** @todo need to probably output if single identifier change */
                if(is_expr_syntax_change())
                    ++number_parts_report;

                if(identifier_rename_only && identifier_renames.size() == 1)
                    identifier_rename = identifier_renames.begin()->get();

            }

        }

        if(number_parts_report == 0) return;

    }

    if(number_parts_report == 1 && bool(identifier_rename))
        summaries_.emplace_back(new identifier_summary_t(*identifier_rename, false));
    else
        summaries_.emplace_back(new decl_stmt_summary_t(profile->operation, decl_stmt_profile->specifiers.size() != 0,
                                                                            decl_stmt_profile->type->syntax_count > 0,
                                                                            !decl_stmt_profile->name.is_common(),
                                                                            bool(decl_stmt_profile->init) && decl_stmt_profile->init->syntax_count > 0));

}

void summary_list::exception(const std::shared_ptr<profile_t> & profile) {

    assert(is_exception_handling(profile->type_name));

    if(profile->operation != SRCDIFF_COMMON)
        summaries_.emplace_back(new exception_summary_t(profile->operation, get_type_string(profile)));

    identifiers(profile->declarations, profile->identifiers);

    block(profile);

}

void summary_list::label(const std::shared_ptr<profile_t> & profile) {

    assert(is_label(profile->type_name));

    summaries_.emplace_back(new label_summary_t(profile->operation, get_type_string(profile)));

}

void summary_list::macro(const std::shared_ptr<profile_t> & profile) {

    assert(typeid(*profile.get()) == typeid(call_profile_t));

    const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(profile);

    summaries_.emplace_back(new macro_summary_t(profile->operation));

}

summary_list::summary_list() {}

summary_list::~summary_list() {

    for(summary_t * summary : summaries_)
        delete summary;

}

void summary_list::function_body(const profile_t & profile) {

    identifiers(profile.declarations, profile.identifiers);

    block(std::make_shared<profile_t>(profile));

}

const std::list<summary_t *> & summary_list::summaries() const {

    return summaries_;

}

std::list<summary_t *> & summary_list::summaries() {

    return summaries_;

}
