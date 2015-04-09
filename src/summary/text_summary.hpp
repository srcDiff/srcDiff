#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <call_profile_t.hpp>
#include <change_entity_map.hpp>
#include <summary_output_stream.hpp>

#include <cstdlib>
#include <vector>
#include <memory>
#include <map>

class text_summary {

private:

protected:

        const size_t id;

        const profile_t::profile_list_t & child_profiles;

        const change_entity_map<parameter_profile_t> & parameters;
        const change_entity_map<call_profile_t>      & member_initializations;

        const std::map<versioned_string, size_t> & summary_identifiers;

        std::map<versioned_string, size_t> output_identifiers;

private:

    std::string get_article(const std::string & type_name) const;
    std::string get_article(const std::shared_ptr<profile_t> & profile) const;
    std::string get_type_string(const std::shared_ptr<profile_t> & profile) const;
    std::string get_profile_string(const std::shared_ptr<profile_t> & profile) const;

    summary_output_stream & identifiers(summary_output_stream & out, const std::map<versioned_string, size_t> & identifiers);
    summary_output_stream & replacement(summary_output_stream & out, const std::shared_ptr<profile_t> & profile, size_t & pos) const;

    bool is_body_summary(const std::string & type, bool is_replacement) const;

public:

    text_summary(const size_t id, const profile_t::profile_list_t & child_profiles, const change_entity_map<parameter_profile_t> & parameters,
                 const change_entity_map<call_profile_t> & member_initializations,
                 const std::map<versioned_string, size_t> & summary_identifiers);

    summary_output_stream & parameter(summary_output_stream & out, size_t number_parameters_deleted,
                            size_t number_parameters_inserted, size_t number_parameters_modified) const;
    summary_output_stream & member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                         size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const;
    void expr_stmt_call(const std::shared_ptr<profile_t> & profile, const std::map<versioned_string, size_t> & identifier_set,
                        std::vector<std::shared_ptr<call_profile_t>> & deleted_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & inserted_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & modified_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & renamed_calls,
                        std::vector<std::shared_ptr<call_profile_t>> & modified_argument_lists,
                        std::vector<std::vector<std::shared_ptr<call_profile_t>>> & argument_list_modifications) const;
    std::string summarize_calls(std::vector<std::shared_ptr<call_profile_t>> & deleted_calls,
                                std::vector<std::shared_ptr<call_profile_t>> & inserted_calls,
                                std::vector<std::shared_ptr<call_profile_t>> & modified_calls,
                                std::vector<std::shared_ptr<call_profile_t>> & renamed_calls,
                                std::vector<std::shared_ptr<call_profile_t>> & modified_argument_lists,
                                std::vector<std::vector<std::shared_ptr<call_profile_t>>> & argument_list_modifications) const;
    summary_output_stream & expr_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) const;
    summary_output_stream & decl_stmt(summary_output_stream & out, const std::shared_ptr<profile_t> & profile) const;
    summary_output_stream & else_clause(summary_output_stream & out, const std::shared_ptr<profile_t> & profile);
    summary_output_stream & conditional(summary_output_stream & out, const std::shared_ptr<profile_t> & profile);
    summary_output_stream & interchange(summary_output_stream & out, const std::shared_ptr<profile_t> & profile);
    summary_output_stream & body(summary_output_stream & out, const profile_t & profile);

};

#endif