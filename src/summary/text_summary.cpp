#include <text_summary.hpp>

#include <conditional_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <call_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <if_profile_t.hpp>
#include <call_profile_t.hpp>
#include <expr_profile_t.hpp>
#include <identifier_profile_t.hpp>
#include <identifier_diff.hpp>
#include <summary_manip.hpp>

#include <algorithm>
#include <functional>
#include <list>
#include <set>
#include <typeinfo>

const char * const BOLD_TEXT = "\x1b[1m";
const char * const NORMAL_TEXT = "\x1b[0m";
std::string bold(std::string text) {

    return BOLD_TEXT + text + NORMAL_TEXT;

}

std::string text_summary::get_article(const std::string & type_name) const { 

    const char letter = type_name[0];

    if(letter == 'a' || letter == 'e' || letter == 'i' || letter == 'o' || letter == 'u')
        return "an";
    else
        return "a";
}

std::string text_summary::get_article(const std::shared_ptr<profile_t> & profile) const { 

    const bool is_guard_clause = profile->type_name == "if" ? reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile)->is_guard() : false;
    if(is_guard_clause) return "a";

    if(is_expr_stmt(profile->type_name)
        && (reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile)->is_delete()
            || reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile)->call()))
        return "a";

    return get_article(std::string(profile->type_name));

}

std::string text_summary::get_type_string(const std::shared_ptr<profile_t> & profile) const {

    if(profile->type_name == "if") {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);
        if(if_profile->is_guard()) return bold("guard clause");

    }

    if(profile->type_name == "else") return bold("else");

    if(is_decl_stmt(profile->type_name)) return bold("declaration");

    if(is_expr_stmt(profile->type_name)) {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);
        if(expr_stmt_profile->assignment()) return bold("assignment");
        if(expr_stmt_profile->is_delete())  return bold("delete");
        if(expr_stmt_profile->call()) {
            std::vector<std::shared_ptr<call_profile_t>>::size_type number_calls = expr_stmt_profile->get_call_profiles().size();
            if(number_calls == 1)           return bold("call");
            else                            return bold("call chain");
        }
        return bold("expression");

    }

    if(is_comment(profile->type_name)) return bold(profile->type_name);

    return bold(profile->type_name);

}

std::string text_summary::get_type_string_with_count(const std::shared_ptr<profile_t> & profile) const {

    if(!has_body(profile->type_name) || profile->operation == SRCDIFF_COMMON
        || (profile->statement_count == 1 && profile->common_statements == 1)) return get_type_string(profile);

    if(profile->type_name == "if") {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);
        if(if_profile->is_guard()) return "guard clause";

    }

    size_t statement_count = profile->operation == SRCDIFF_DELETE ? profile->statement_count_original : profile->statement_count_modified;
    if(profile->type_name == "elseif") --statement_count;

    if(statement_count == 0)
        return "empty " + get_type_string(profile);

    if(profile->common_statements > 0 && profile->common_statements != statement_count)
        return get_type_string(profile);

    if(statement_count == 1)
        return get_type_string(profile) + " with a single statement";

    return get_type_string(profile) + " with " + std::to_string(statement_count) + " statements";   

}

std::string text_summary::get_profile_string(const std::shared_ptr<profile_t> & profile) const {

    if(!profile->type_name.is_common()) {

        std::string original = get_article(profile->type_name.original()) + " " + bold(profile->type_name.original());
        std::string modified = get_article(profile->type_name.modified()) + " " + bold(profile->type_name.modified());

        return original + " was converted to " + modified;

    }

    if(profile->type_name == "if") {

        const std::shared_ptr<if_profile_t> & if_profile = reinterpret_cast<const std::shared_ptr<if_profile_t> &>(profile);

        size_t statement_count = profile->operation == SRCDIFF_DELETE ? profile->statement_count_original : profile->statement_count_modified;
        if(profile->type_name == "elseif") --statement_count;

        if(if_profile->else_clause() && if_profile->operation != SRCDIFF_COMMON)
            return "an " + bold("if-else");

    }

    if(is_expr_stmt(profile->type_name)) {

        const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

        if(expr_stmt_profile->assignment() && expr_stmt_profile->operation != SRCDIFF_COMMON) {

            std::string expr_stmt_summary = "an " + bold("assignment");

            if(abstract_level != HIGH) {

                expr_stmt_summary += " to '";
                if(expr_stmt_profile->operation == SRCDIFF_DELETE)      expr_stmt_summary += expr_stmt_profile->lhs().original() + '\'';
                else if(expr_stmt_profile->operation == SRCDIFF_INSERT) expr_stmt_summary += expr_stmt_profile->lhs().modified() + '\'';

            }

            return expr_stmt_summary;

        }

        if(expr_stmt_profile->call()) {

                std::string expr_stmt_summary;
                if(expr_stmt_profile->get_call_profiles().size() == 1) {

                    expr_stmt_summary = "a " + bold("call");

                    if(abstract_level != HIGH) {
    
                        expr_stmt_summary += " to '";
                        const std::shared_ptr<call_profile_t> & call_profile = expr_stmt_profile->get_call_profiles()[0];
                        if(expr_stmt_profile->operation == SRCDIFF_DELETE)      expr_stmt_summary += call_profile->name.original() + '\'';
                        else if(expr_stmt_profile->operation == SRCDIFF_INSERT) expr_stmt_summary += call_profile->name.modified() + '\'';

                    }

                } else {

                    expr_stmt_summary = "a " + bold("chain of calls");

                }

                return expr_stmt_summary;

        }

    }

    if(is_call(profile->type_name)) {

        const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(profile);
        return "a " + bold("call") + " to '" + call_profile->name.original() + "' was renamed to '" + call_profile->name.modified();

    }

    return get_article(profile) + ' ' + get_type_string(profile);

}

summary_output_stream & text_summary::identifiers(summary_output_stream & out, const std::map<identifier_diff, size_t> & identifiers) {

    for(std::pair<identifier_diff, size_t> identifier : identifiers) {

        if(identifier.second <= 1) continue;

        out.begin_line();// << "the identifier '";

        if(identifier.first.complex()) {

            out << "name change from '";
            out << identifier.first.get_diff().original();
            out << "' to '";
            out << identifier.first.get_diff().modified();
            out << '\'';


        } else {

            out << '\'';
            out << identifier.first.get_diff().original();
            out << "' was renamed to '";
            out << identifier.first.get_diff().modified();
            out << '\'';

        }

        out << '\n';

    }

    for(std::map<identifier_diff, size_t>::const_iterator itr = identifiers.begin(); itr != identifiers.end(); ++itr) {

        std::map<identifier_diff, size_t>::iterator itersect_itr = output_identifiers.find(itr->first);
        if(itersect_itr == output_identifiers.end())
            output_identifiers.insert(itersect_itr, *itr);
        else
            itersect_itr->second += itr->second;

    }

    return out;

}

summary_output_stream & text_summary::replacement(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t & pos, const bool parent_output) const {

    const std::shared_ptr<profile_t> & start_profile = profile->child_profiles[pos];

    std::vector<std::reference_wrapper<const std::shared_ptr<expr_stmt_profile_t>>>   expr_stmt_deleted,    expr_stmt_inserted;
    std::vector<std::reference_wrapper<const std::shared_ptr<decl_stmt_profile_t>>>   decl_stmt_deleted,    decl_stmt_inserted;
    std::vector<std::reference_wrapper<const std::shared_ptr<conditional_profile_t>>> conditionals_deleted, conditionals_inserted;
    std::vector<std::reference_wrapper<const std::shared_ptr<profile_t>>>             jump_deleted,         jump_inserted;
    std::vector<std::reference_wrapper<const std::shared_ptr<profile_t>>>             comment_deleted,      comment_inserted;
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

    out.begin_line();

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

        if(expr_stmt_deleted.size())
            out << get_profile_string(expr_stmt_deleted.back().get());
        else if(expr_stmt_inserted.size())
            out << get_profile_string(expr_stmt_inserted.back().get());
        else if(decl_stmt_deleted.size())
            out << get_profile_string(decl_stmt_deleted.back().get());
        else if(decl_stmt_inserted.size())
            out << get_profile_string(decl_stmt_inserted.back().get());
        else if(conditionals_deleted.size())
            out << get_profile_string(conditionals_deleted.back().get());
        else if(conditionals_inserted.size())
            out << get_profile_string(conditionals_inserted.back().get());
        else if(jump_deleted.size())
            out << get_profile_string(jump_deleted.back().get());
        else if(jump_inserted.size())
            out << get_profile_string(jump_inserted.back().get());

        out << " was ";

        if(comment_deleted.size())
            out << "uncommented\n";
        else
            out << "commented out\n";

        return out;

    }

    if(number_syntax_deletions == 1) {

        if(expr_stmt_deleted.size())
            out << get_profile_string(expr_stmt_deleted.back().get());
        else if(decl_stmt_deleted.size())
            out << get_profile_string(decl_stmt_deleted.back().get());
        else if(conditionals_deleted.size())
            out << get_profile_string(conditionals_deleted.back().get());
        else if(jump_deleted.size())
            out << get_profile_string(jump_deleted.back().get());
        else
            out << get_profile_string(comment_deleted.back().get());

        out << " was";

    } else {

        if(number_deleted_types == 1 || (comment_deleted.size() != 0 && number_deleted_types == 2)) {

            if(expr_stmt_deleted.size()) {

                out << (expr_stmt_deleted.size() == 1 ? get_article(expr_stmt_deleted.back().get()) :  std::to_string(expr_stmt_deleted.size())) << ' ' << get_type_string(expr_stmt_deleted.back().get());

            } else if(decl_stmt_deleted.size()) {

                out << (decl_stmt_deleted.size() == 1 ? get_article(decl_stmt_deleted.back().get()) :  std::to_string(decl_stmt_deleted.size())) << ' ' << get_type_string(decl_stmt_deleted.back().get());

            } else if(conditionals_deleted.size()) {

                out << (conditionals_deleted.size() == 1 ? get_article(conditionals_deleted.back().get()) :  std::to_string(conditionals_deleted.size())) << ' ' << get_type_string(conditionals_deleted.back().get());

            } else if(jump_deleted.size()) {

                if(jump_deleted.size() == 1) {

                    out << get_profile_string(jump_deleted.back().get());

                } else {

                    std::set<std::string> jump_types;
                    for(const std::shared_ptr<profile_t> & profile_ptr : jump_deleted)
                        jump_types.insert(profile_ptr->type_name.original());

                    if(jump_types.size() == 1)
                        out << std::to_string(jump_deleted.size()) << ' ' << get_type_string(jump_deleted.back());
                    else
                        out << std::to_string(jump_deleted.size()) << " statements";

                }


            }

            if(comment_deleted.size() != 0) {

                if(number_deleted_types == 2)
                    out << " and ";

                out << (comment_deleted.size() == 1 ? "a comment" : std::to_string(comment_deleted.size()) + " comments");

            }

        } else {

            out << std::to_string(number_syntax_deletions) << " statements";

            if(comment_deleted.size() != 0)
                out << " and " << (comment_deleted.size() == 1 ? "a comment" : std::to_string(comment_deleted.size()) + " comments");

        }

        out << " were";

    }

    out << " replaced with ";

    if(number_syntax_insertions == 1) {

        if(expr_stmt_inserted.size())
            out << get_profile_string(expr_stmt_inserted.back().get());
        else if(decl_stmt_inserted.size())
            out << get_profile_string(decl_stmt_inserted.back().get());
        else if(conditionals_inserted.size())
            out << get_profile_string(conditionals_inserted.back().get());
        else if(jump_inserted.size())
            out << get_profile_string(jump_inserted.back().get());
        else
            out << get_profile_string(comment_inserted.back().get());

    } else {

        if(number_inserted_types == 1 || (comment_inserted.size() != 0 && number_inserted_types == 2)) {

            if(expr_stmt_inserted.size()) {

                out << (expr_stmt_inserted.size() == 1 ? get_article(expr_stmt_inserted.back().get()) :  std::to_string(expr_stmt_inserted.size())) << ' ' << get_type_string(expr_stmt_inserted.back().get());

            } else if(decl_stmt_inserted.size()) {

                out << (decl_stmt_inserted.size() == 1 ? get_article(decl_stmt_inserted.back().get()) :  std::to_string(decl_stmt_inserted.size())) << ' ' << get_type_string(decl_stmt_inserted.back().get());

            } else if(conditionals_inserted.size()) {

                out << (conditionals_inserted.size() == 1 ? get_article(conditionals_inserted.back().get()) :  std::to_string(conditionals_inserted.size())) << ' ' << get_type_string(conditionals_inserted.back().get());

            } else if(jump_inserted.size()) {

                if(jump_inserted.size() == 1) {

                    out << get_profile_string(jump_inserted.back().get());

                } else {

                    std::set<std::string> jump_types;
                    for(const std::shared_ptr<profile_t> & profile_ptr : jump_inserted)
                        jump_types.insert(profile_ptr->type_name.original());

                    if(jump_types.size() == 1)
                        out << std::to_string(jump_inserted.size()) << ' ' << get_type_string(jump_inserted.back());
                    else
                        out << std::to_string(jump_inserted.size()) << " statements";

                }


            }

            if(comment_inserted.size() != 0) {

                if(number_inserted_types == 2)
                    out << " and ";

                out << (comment_inserted.size() == 1 ? "a comment" : std::to_string(comment_inserted.size()) + " comments");

            }

        } else {

            out << std::to_string(number_syntax_insertions) << " statements";

            if(comment_deleted.size() != 0)
                out << " and " << (comment_deleted.size() == 1 ? "a comment" : std::to_string(comment_deleted.size()) + " comments");

        }

    }

    out << '\n';

    return out;

}

bool text_summary::is_body_summary(const std::string & type, bool is_replacement) const {

    return is_condition_type(type) || is_expr_stmt(type) || is_decl_stmt(type) || (is_comment(type) && is_replacement)
        || is_jump(type);

}

summary_output_stream & text_summary::statement_dispatch(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t & child_pos, const bool parent_output) {

    const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[child_pos];

    if(child_profile->is_replacement && ((child_pos + 1) < profile->child_profiles.size())) {

        replacement(out, profile, child_pos, parent_output);

    } else if(child_profile->move_id) {

        out.begin_line() << get_profile_string(child_profile) << " was moved\n";

    } else if(!child_profile->type_name.is_common()) {

        interchange(out, child_profile, parent_output);

    } else {

        if(is_jump(child_profile->type_name))
            jump(out, child_profile, parent_output);
        else if(is_condition_type(child_profile->type_name))
            conditional(out, child_profile, parent_output);
        else if(is_expr_stmt(child_profile->type_name))
            expr_stmt(out, child_profile, parent_output);
        else if(is_decl_stmt(child_profile->type_name))
            decl_stmt(out, child_profile, parent_output);

    }

    return out;

}

size_t text_summary::number_child_changes(const profile_t::profile_list_t & child_profiles) const {

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
            else if(is_expr_stmt(child_profile->type_name))
                ++num_child_changes;
            else if(is_decl_stmt(child_profile->type_name))
                ++num_child_changes;

        }

    }

    return num_child_changes > 0 ? num_child_changes + num_body_changes : num_child_changes;

}

text_summary::text_summary(const size_t id, const profile_t::profile_list_t & child_profiles, const change_entity_map<parameter_profile_t> & parameters,
             const change_entity_map<call_profile_t> & member_initializations,
             const std::map<identifier_diff, size_t> & summary_identifiers,
             abstraction_level abstract_level)
    : id(id), child_profiles(child_profiles), parameters(parameters), member_initializations(member_initializations),
      summary_identifiers(summary_identifiers), body_depth(0), abstract_level(abstract_level) {}

summary_output_stream & text_summary::parameter(summary_output_stream & out, size_t number_parameters_deleted,
                                       size_t number_parameters_inserted, size_t number_parameters_modified) const {

    if(number_parameters_deleted > 0) {

        out.begin_line();

        if(number_parameters_deleted == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was removed";

        else
            out << std::to_string(number_parameters_deleted) << ' ' << manip::bold() << "parameters" << manip::normal() << " were removed:\n";

    }

    if(number_parameters_inserted > 0) {

        out.begin_line();

        if(number_parameters_inserted == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was added\n";
        else
            out << std::to_string(number_parameters_inserted) << ' ' << manip::bold() << "parameters" << manip::normal() << " were added:\n";

    }

    if(number_parameters_modified > 0) {

        out.begin_line();

        if(number_parameters_modified == 1)
            out << "a " << manip::bold() << "parameter" << manip::normal() << " was modified\n";
        else
            out << std::to_string(number_parameters_modified) << ' ' << manip::bold() << "parameters" << manip::normal() << " were modified:\n";

    }

    return out;

}

summary_output_stream & text_summary::member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                                   size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const {

    if(number_member_initializations_deleted > 0) {

        out.begin_line();

        if(number_member_initializations_deleted == 1)
            out << "a " << manip::bold() << "member initilization" << manip::normal() << " was removed\n";
        else
            out << std::to_string(number_member_initializations_deleted) << ' ' << manip::bold() << "member initilizations" << manip::normal() << " were removed:\n";

    }

    if(number_member_initializations_inserted > 0) {

        out.begin_line();

        if(number_member_initializations_inserted == 1)
            out << "a " << manip::bold() << "member initilization" << manip::normal() << " was added\n";
        else
            out << std::to_string(number_member_initializations_inserted) << ' ' << manip::bold() << "member initilizations" << manip::normal() << " were added:\n";

    }

    if(number_member_initializations_modified > 0) {

        out.begin_line();

        if(number_member_initializations_modified == 1)
            out << "a " << manip::bold() << "member initilization" << manip::normal() << " was modified\n";
        else
            out << std::to_string(number_member_initializations_modified) << ' ' << manip::bold() << "member initilizations" << manip::normal() << " were modified:\n";

    }

    return out;

}

void text_summary::expr_statistics(const std::shared_ptr<profile_t> & profile, const std::map<identifier_diff, size_t> & identifier_set,
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
                              std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    assert(typeid(*profile.get()) == typeid(expr_profile_t));

    for(size_t pos = 0; pos < profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        if(child_profile->operation == SRCDIFF_COMMON && child_profile->syntax_count == 0) continue;

        if(child_profile->type_name.is_common() && is_call(child_profile->type_name)) {

            const std::shared_ptr<call_profile_t> & call_profile = reinterpret_cast<const std::shared_ptr<call_profile_t> &>(child_profile);

            if(child_profile->operation == SRCDIFF_DELETE) {

                deleted_calls.push_back(call_profile);

            } else if(child_profile->operation == SRCDIFF_INSERT) {

                inserted_calls.push_back(call_profile);

            } else if(child_profile->operation == SRCDIFF_COMMON) {

                bool report_name = !call_profile->name.is_common();
                if(report_name) {

                    identifier_diff ident_diff(call_profile->name);
                    ident_diff.trim(true);

                    if(!identifier_set.count(ident_diff)) {
                        report_name = false;
                    }

                }

                bool report_argument_list = call_profile->argument_list_modified;
                if(report_argument_list) {

                    size_t number_deleted  = call_profile->arguments.count(SRCDIFF_DELETE);
                    size_t number_inserted = call_profile->arguments.count(SRCDIFF_INSERT);

                    number_arguments_deleted  += number_deleted;
                    number_arguments_inserted += number_inserted;

                    bool report_change = false;
                    std::for_each(call_profile->arguments.lower_bound(SRCDIFF_COMMON), call_profile->arguments.upper_bound(SRCDIFF_COMMON),
                        [&, this](const typename change_entity_map<profile_t>::pair & pair) {

                            if(pair.second->syntax_count == 0) return;

                            for(const std::shared_ptr<profile_t> & argument_child_profile : pair.second->child_profiles[0]->child_profiles) {

                               if(argument_child_profile->type_name.is_common() && is_call(argument_child_profile->type_name)) {

                                    size_t num_calls = 0;
                                    std::vector<std::shared_ptr<call_profile_t>> inner_deleted_calls, inner_inserted_calls,
                                        inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists;
                                    std::vector<std::shared_ptr<profile_t>> inner_deleted_other, inner_inserted_other, inner_modified_other;
                                    size_t inner_number_arguments_deleted = 0, inner_number_arguments_inserted = 0, inner_number_arguments_modified = 0;
                                    expr_statistics(argument_child_profile->parent, identifier_set,
                                                    inner_deleted_calls, inner_inserted_calls, inner_modified_calls, inner_renamed_calls, inner_modified_argument_lists,
                                                    inner_deleted_other, inner_inserted_other, inner_modified_other,
                                                    inner_number_arguments_deleted, inner_number_arguments_inserted, inner_number_arguments_modified,
                                                    identifier_renames);

                                    if(inner_deleted_calls.size() || inner_inserted_calls.size()
                                        || inner_modified_calls.size() || inner_renamed_calls.size() || inner_modified_argument_lists.size()) {

                                        report_change = true;
                                        break;

                                    }

                                } else if(argument_child_profile->operation != SRCDIFF_COMMON) { 

                                    report_change = true;
                                    break;

                                } else if(!is_identifier(argument_child_profile->type_name)) {

                                    report_change = true;
                                    break;

                                } else {

                                    const std::shared_ptr<identifier_profile_t> & identifier_profile
                                        = reinterpret_cast<const std::shared_ptr<identifier_profile_t> &>(argument_child_profile);

                                    identifier_diff ident_diff(identifier_profile->name);
                                    ident_diff.trim(false);

                                    if(identifier_set.count(ident_diff)) {

                                        report_change = true;
                                        identifier_renames.insert(identifier_profile->name);
                                        break;

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

        }

     }

}

summary_output_stream & text_summary::common_expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) const {

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    std::map<identifier_diff, size_t> diff_set;
    std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
                        output_identifiers.begin(), output_identifiers.end(),
                        std::inserter(diff_set, diff_set.begin()));

    std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists;
    std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;
    size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;
    std::set<std::reference_wrapper<const versioned_string>> identifier_renames; 
    expr_statistics(profile->child_profiles[0], diff_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,
                    deleted_other, inserted_other, modified_other,
                    number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
                    identifier_renames);

    if(deleted_calls.size() == 0 && inserted_calls.size() == 0 && modified_calls.size() == 0) return out;

    if(expr_stmt_profile->call())
        return call_sequence(out, profile, renamed_calls.size(), number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
                             modified_argument_lists.size(), identifier_renames);

    out.begin_line();

    size_t number_change_types = 0;
    if(deleted_calls.size() != 0)           ++number_change_types;
    if(inserted_calls.size() != 0)          ++number_change_types;
    if(renamed_calls.size() != 0)           ++number_change_types;
    if(modified_argument_lists.size() != 0) ++number_change_types;

    if(number_change_types == 1) {

        if(deleted_calls.size() != 0) {

            if(deleted_calls.size() == 1)
                out << "a " << manip::bold() << "call" << manip::normal() << " was deleted";
            else
                out << std::to_string(deleted_calls.size()) << ' ' << manip::bold() << "calls" << manip::normal() << " were deleted";

        } else if(inserted_calls.size() != 0) {

            if(inserted_calls.size() == 1)
                out << "a " << manip::bold() << "call" << manip::normal() << " was added";
            else
                out << std::to_string(inserted_calls.size()) << ' ' << manip::bold() << "calls" << manip::normal() << " were added";

        } else if(renamed_calls.size() != 0) {

            if(renamed_calls.size() == 1)
                out << "a " << manip::bold() << "call" << manip::normal() << " was renamed";
            else
                out << std::to_string(renamed_calls.size()) << ' ' << manip::bold() << "calls" << manip::normal() << " were renamed";

        } else if(modified_argument_lists.size() != 0) {

            if(modified_argument_lists.size() == 1) {

                if(modified_argument_lists[0]->child_profiles.size() == 1)
                    out << "an " << manip::bold() << "argument" << manip::normal() << " was modified";
                else
                    out << std::to_string(modified_argument_lists[0]->child_profiles.size()) << manip::bold() << "arguments" << manip::normal() << " were modified";

            } else {

                out << manip::bold() << "argument lists" << manip::normal() << " were modified";

            }

        }

    } else {

       out << get_profile_string(profile) << " was modified";

    }

    out << '\n';

    return out;

}

bool operator<(const std::__1::reference_wrapper<const versioned_string> & ref_one, const std::__1::reference_wrapper<const versioned_string> & ref_two) {

    return ref_one.get() < ref_two.get();

}

summary_output_stream & text_summary::call_sequence(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t number_rename,
                                                    size_t number_arguments_deleted, size_t number_arguments_inserted, size_t number_arguments_modified,
                                                    size_t number_argument_lists_modified, const std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const {

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    std::vector<std::shared_ptr<call_profile_t>>::size_type calls_sequence_length = expr_stmt_profile->get_call_profiles().size();

    size_t number_arguments_total = number_arguments_deleted + number_arguments_inserted + number_arguments_modified;
    bool is_variable_reference_change = number_argument_lists_modified == 0;

    if(is_variable_reference_change) {

        for(std::vector<std::shared_ptr<call_profile_t>>::size_type pos = 0; pos < calls_sequence_length; ++pos) {

            const std::shared_ptr<call_profile_t> & call_profile = expr_stmt_profile->get_call_profiles()[pos];

            if(call_profile->operation != SRCDIFF_COMMON && pos != (calls_sequence_length - 1)) {

                is_variable_reference_change = false;
                break;

            }

       }

    }

    out.begin_line();

    if(number_rename == 1 && number_argument_lists_modified == 0) {

        out << "a " << manip::bold() << "call name" << manip::normal() << " change occurred";

    } else if(is_variable_reference_change) {

        out << "a " << manip::bold() << "variable reference" << manip::normal() << " change occurred";

    } else if(identifier_renames.size() == 1) {

        out << '\'' << identifier_renames.begin()->get().original() << "' was renamed to '" << identifier_renames.begin()->get().modified() << '\'';

    } else if(number_argument_lists_modified == 1 && number_rename == 0) {

        if(number_arguments_total == 1) {

            out << "an " << manip::bold() << "argument" << manip::normal() << " was ";

            if(number_arguments_deleted == 1)
                out << "removed";
            else if(number_arguments_inserted == 1)
                out << "added";
            else
                out << "modified";

        } else {

           out << "an " << manip::bold() << "argument list" << manip::normal() << " was modified";

        }

    } else {

        out << get_profile_string(profile) << " was modified";

    }

    out << '\n';

    return out;

}

summary_output_stream & text_summary::expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) const {

    const std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<const std::shared_ptr<expr_stmt_profile_t> &>(profile);

    if((expr_stmt_profile->assignment() && expr_stmt_profile->operation != SRCDIFF_COMMON) || expr_stmt_profile->is_delete() || profile->child_profiles.empty()) {

        out.begin_line() << get_profile_string(profile) << " was ";

        out << (profile->operation == SRCDIFF_DELETE ?  "deleted" : (profile->operation == SRCDIFF_INSERT ? "added" : "modified"));

        if(abstract_level != HIGH && (profile->parent == id || !parent_output)) {

            if(profile->operation == SRCDIFF_DELETE)      out << " from ";
            else if(profile->operation == SRCDIFF_INSERT) out << " to ";
            else                                          out << " within ";

            if(profile->parent == id)
                out << "the function";
            else
                out << "a nested " << get_type_string(profile->parent);

        }

        out << '\n';

        return out;

    }

    if(profile->operation == SRCDIFF_COMMON) {

        common_expr_stmt(out, profile);

    }

    return out;

}

/** @todo for decl_stmt and jump need to not report if only a known rename identifier occurs.  Also, report a rename if it occurred */
summary_output_stream & text_summary::decl_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) const {

    const std::shared_ptr<decl_stmt_profile_t> & decl_stmt_profile = reinterpret_cast<const std::shared_ptr<decl_stmt_profile_t> &>(profile);

    const std::shared_ptr<profile_t> & parent_profile = profile->parent;
    std::map<identifier_diff, size_t> identifier_set;
    std::set_difference(parent_profile->identifiers.begin(), parent_profile->identifiers.end(),
                        output_identifiers.begin(), output_identifiers.end(),
                        std::inserter(identifier_set, identifier_set.begin()));

    if(decl_stmt_profile->operation == SRCDIFF_COMMON) {

        bool report = false;
        if(!decl_stmt_profile->type.is_common()) {

            identifier_diff ident_diff(decl_stmt_profile->type);
            ident_diff.trim(true);

            if(identifier_set.count(ident_diff))
                report = true;

        }

        if(!decl_stmt_profile->name.is_common()) {

            identifier_diff ident_diff(decl_stmt_profile->name);
            ident_diff.trim(true);

            if(identifier_set.count(ident_diff))
                report = true;

        }

        if(!decl_stmt_profile->init.is_common()) {

            std::vector<std::shared_ptr<call_profile_t>> deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists;
            std::vector<std::shared_ptr<profile_t>> deleted_other, inserted_other, modified_other;
            size_t number_arguments_deleted = 0, number_arguments_inserted = 0, number_arguments_modified = 0;
            std::set<std::reference_wrapper<const versioned_string>> identifier_renames; 
            expr_statistics(decl_stmt_profile->child_profiles.back(), identifier_set, deleted_calls, inserted_calls, modified_calls, renamed_calls, modified_argument_lists,
                            deleted_other, inserted_other, modified_other,
                            number_arguments_deleted, number_arguments_inserted, number_arguments_modified,
                            identifier_renames);

            /** @todo need to add support for detecting other changes in expr_statistics and then use to refine here */

            report = true;

        }

        if(!report) return out;

    }

    out.begin_line() << get_profile_string(decl_stmt_profile);

    out << " was ";

    out << (profile->operation == SRCDIFF_DELETE ?  "deleted" : (profile->operation == SRCDIFF_INSERT ? "added" : "modified"));

    if(abstract_level != HIGH && (profile->parent == id || !parent_output)) {

        if(profile->operation == SRCDIFF_DELETE)      out << " from ";
        else if(profile->operation == SRCDIFF_INSERT) out << " to ";
        else                                          out << " within ";

        if(profile->parent == id)
            out << "the function";
        else
            out << "a nested " << get_type_string(profile->parent);

    }

    out << '\n';

    return out;

}

summary_output_stream & text_summary::else_clause(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) {

    if(!profile->type_name.is_common())
        return interchange(out, profile, parent_output);

    const bool has_common = profile->common_profiles.size() > 0;

    const bool output_else = profile->operation != SRCDIFF_COMMON || number_child_changes(profile->child_profiles) > 1;

    if(profile->parent->operation != SRCDIFF_COMMON) {

        out.begin_line();
        out << "an " << manip::bold() << "if-else" << manip::normal() << " was ";
        out << (profile->parent->operation == SRCDIFF_DELETE ? "removed" : "added");
        out << '\n';

        return out;

    }

    if(profile->summary_identifiers.size() > 0) {

        identifiers(out, profile->summary_identifiers);

    }

    if(profile->operation != SRCDIFF_COMMON) {

        out.begin_line();

        if(profile->parent->operation != SRCDIFF_COMMON)
            out << "the " << get_type_string(profile) << " was ";
        else
            out << get_profile_string(profile) << " was ";

        out << (profile->operation == SRCDIFF_DELETE ? "removed" : "added");

        if(abstract_level != HIGH && (profile->parent == id && profile->operation == SRCDIFF_COMMON)) {

            if(profile->operation == SRCDIFF_DELETE)      out << " from ";
            else if(profile->operation == SRCDIFF_INSERT) out << " to ";
            else                                          out << " within ";

            out << "the function";

        }

        out << '\n';

    }

    ++body_depth;

    for(size_t pos = 0; pos < profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        if((child_profile->syntax_count > 0 || child_profile->move_id
            || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
            && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            statement_dispatch(out, profile, pos, output_else);

        }

    }

    --body_depth;

    return out;

}

/** @todo if multiple of same change like test case where connect deleted 4 times.  May want to some in one line. */
summary_output_stream & text_summary::conditional(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) {

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
        return else_clause(out, profile->child_profiles[0], parent_output);

    const std::shared_ptr<profile_t> & summary_profile = profile->type_name == "elseif" && profile->child_profiles.size() == 1
        && profile->child_profiles[0]->type_name == "if" ? profile->child_profiles[0] : profile;

    const bool output_conditional = summary_profile->operation != SRCDIFF_COMMON || condition_modified || number_child_changes(summary_profile->child_profiles) > 1;

    size_t statement_count = summary_profile->operation == SRCDIFF_DELETE ? summary_profile->statement_count_original : summary_profile->statement_count_modified;
    if(profile->type_name == "elseif") --statement_count;
    const size_t common_statements = summary_profile->common_statements;

    if(condition_modified) {

         out.begin_line() << "the condition of " << get_profile_string(profile) << " was altered\n";

    }

    if(summary_profile->summary_identifiers.size() > 0) {

        identifiers(out, summary_profile->summary_identifiers);

    }

    // before children
    bool is_leaf = true;
    if(profile->operation != SRCDIFF_COMMON) {

        out.begin_line();

        out << get_profile_string(profile);
        out << " was ";
        out << (profile->operation == SRCDIFF_DELETE ? "removed" : "added");

        // after children
        if(abstract_level != HIGH && (summary_profile->parent == id && summary_profile->operation == SRCDIFF_COMMON)) {

            if(summary_profile->operation == SRCDIFF_DELETE)      out << " from ";
            else if(summary_profile->operation == SRCDIFF_INSERT) out << " to ";
            else                                          out << " within ";

            out << "the function";

        }

        out << '\n';

    }

    ++body_depth;

    /** todo should I only report if one expr_stmt modified, what if expression statement after condition both having been modified */
    for(size_t pos = 0; pos < summary_profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = summary_profile->child_profiles[pos];

        if((child_profile->syntax_count > 0 || child_profile->move_id
            || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
            && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            statement_dispatch(out, summary_profile, pos, output_conditional);

        }

    }

    --body_depth;

    return out;

}

summary_output_stream & text_summary::interchange(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) {

    out.begin_line();

    out << get_profile_string(profile) << '\n';

    bool is_leaf = true;

    ++body_depth;

    std::shared_ptr<profile_t> summary_profile = profile;
    if(profile->type_name.original() == "elseif" || profile->type_name.modified() == "elseif")
        summary_profile = profile->child_profiles[0];

    for(size_t pos = 0; pos < summary_profile->child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = profile->child_profiles[pos];

        if((child_profile->syntax_count > 0 || child_profile->move_id
            || (child_profile->operation != SRCDIFF_COMMON && profile->operation != child_profile->operation))
            && is_body_summary(child_profile->type_name, child_profile->is_replacement)) {

            statement_dispatch(out, profile, pos, true);

        }

    }

    --body_depth;

    return out;

}


summary_output_stream & text_summary::jump(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) const {

    out.begin_line() << get_profile_string(profile);

    out << " was ";

    out << (profile->operation == SRCDIFF_DELETE ?  "deleted" : (profile->operation == SRCDIFF_INSERT ? "added" : "modified"));

    if(abstract_level != HIGH && (profile->parent == id || !parent_output)) {

        if(profile->operation == SRCDIFF_DELETE)      out << " from ";
        else if(profile->operation == SRCDIFF_INSERT) out << " to ";
        else                                          out << " within ";

        if(profile->parent == id)
            out << "the function";
        else
            out << "a nested " << get_type_string(profile->parent);

    }

    out << '\n';

    return out;

}

summary_output_stream & text_summary::body(summary_output_stream & out, const profile_t & profile) {

    identifiers(out, summary_identifiers);

    for(size_t pos = 0; pos < child_profiles.size(); ++pos) {

        const std::shared_ptr<profile_t> & child_profile = child_profiles[pos];

        if(!is_body_summary(child_profile->type_name, child_profile->is_replacement)
            || (child_profile->operation == SRCDIFF_COMMON && child_profile->syntax_count == 0
                && child_profile->move_id == 0))
            continue;

        statement_dispatch(out, std::make_shared<profile_t>(profile), pos, true);

    }

    return out;

}
