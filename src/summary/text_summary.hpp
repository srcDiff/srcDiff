#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <call_profile_t.hpp>
#include <change_entity_map.hpp>

#include <cstdlib>
#include <iostream>
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

    std::string get_article(const std::shared_ptr<profile_t> & profile) const;
    std::string get_type_string(const std::shared_ptr<profile_t> & profile) const;
    std::string get_profile_string(const std::shared_ptr<profile_t> & profile) const;

    std::ostream & identifiers(std::ostream & out, const std::map<versioned_string, size_t> & identifiers);
    std::ostream & replacement(std::ostream & out, const std::shared_ptr<profile_t> & profile, size_t & pos) const;

    bool is_body_summary(const std::string & type, bool is_replacement) const;

public:

    text_summary(const size_t id, const profile_t::profile_list_t & child_profiles, const change_entity_map<parameter_profile_t> & parameters,
                 const change_entity_map<call_profile_t> & member_initializations,
                 const std::map<versioned_string, size_t> & summary_identifiers,
                 const std::vector<std::shared_ptr<profile_t>> & profile_list);

    std::ostream & parameter(std::ostream & out, size_t number_parameters_deleted,
                            size_t number_parameters_inserted, size_t number_parameters_modified) const;
    std::ostream & member_initialization(std::ostream & out, size_t number_member_initializations_deleted,
                                         size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const;
    void call_check(const std::shared_ptr<profile_t> & profile, const std::map<versioned_string, size_t> & identifier_set,
                    size_t & number_calls, size_t & number_renames, size_t & number_argument_list_modified) const;
    std::ostream & expr_stmt(std::ostream & out, const std::shared_ptr<profile_t> & profile) const;
    std::ostream & decl_stmt(std::ostream & out, const std::shared_ptr<profile_t> & profile) const;
    std::ostream & conditional(std::ostream & out, const std::shared_ptr<profile_t> & profile);
    std::ostream & body(std::ostream & out, const profile_t & profile);

};

#endif