#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <call_profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <change_entity_map.hpp>
#include <summary_output_stream.hpp>

#include <cstdlib>
#include <vector>
#include <memory>
#include <map>

class text_summary {

public:

    enum abstraction_level { HIGH, MEDIUM, LOW };

private:

protected:

        const size_t id;

        const profile_t::profile_list_t & child_profiles;

        const change_entity_map<parameter_profile_t> & parameters;
        const change_entity_map<call_profile_t>      & member_initializations;

        const std::map<identifier_diff, size_t> & summary_identifiers;

        std::map<identifier_diff, size_t> output_identifiers;

        size_t body_depth;

        abstraction_level abstract_level;

private:

    std::string get_article(const std::string & type_name) const;
    std::string get_article(const std::shared_ptr<profile_t> & profile) const;
    std::string get_type_string(const std::shared_ptr<profile_t> & profile) const;
    std::string get_type_string_with_count(const std::shared_ptr<profile_t> & profile) const;
    std::string get_profile_string(const std::shared_ptr<profile_t> & profile) const;

    summary_output_stream & identifiers(summary_output_stream & out, const std::map<identifier_diff, size_t> & identifiers);
    summary_output_stream & replacement(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t & pos, const bool parent_output UNUSED) const;

    bool is_body_summary(const std::string & type, bool is_replacement) const;

    summary_output_stream & statement_dispatch(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t & child_pos, const bool parent_output);

    size_t number_child_changes(const profile_t::profile_list_t & child_profiles) const;

public:

    text_summary(const size_t id, const profile_t::profile_list_t & child_profiles, const change_entity_map<parameter_profile_t> & parameters,
                 const change_entity_map<call_profile_t> & member_initializations,
                 const std::map<identifier_diff, size_t> & summary_identifiers,
                 abstraction_level abstract_level = HIGH);

    summary_output_stream & parameter(summary_output_stream & out, size_t number_parameters_deleted,
                            size_t number_parameters_inserted, size_t number_parameters_modified) const;
    summary_output_stream & member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                         size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const;
    void expr_stmt_call(const std::shared_ptr<profile_t> & profile, const std::map<identifier_diff, size_t> & identifier_set,
                        std::vector<std::shared_ptr<call_profile_t>> & deleted_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & inserted_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & modified_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & renamed_calls,
                        size_t & number_arguments_deleted,
                        size_t & number_arguments_inserted,
                        size_t & number_arguments_modified,
                        size_t & number_arguments_total,
                        std::vector<std::shared_ptr<call_profile_t>> & modified_argument_lists,
                        std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const;
    summary_output_stream & common_expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & expr_stmt_profile) const;
    summary_output_stream & call_sequence(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t number_rename,
                                          size_t number_arguments_deleted, size_t number_arguments_inserted, size_t numbe_arguments_modified,
                                          size_t number_argument_lists_modified, const std::set<std::reference_wrapper<const versioned_string>> & identifier_renames) const;
    summary_output_stream & expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) const;
    summary_output_stream & decl_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) const;
    summary_output_stream & else_clause(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output UNUSED);
    summary_output_stream & conditional(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output UNUSED);
    summary_output_stream & interchange(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output UNUSED);
    summary_output_stream & jump(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, const bool parent_output) const;
    summary_output_stream & body(summary_output_stream & out, const profile_t & profile);

};

#endif