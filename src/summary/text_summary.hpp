#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <parameter_profile_t.hpp>
#include <call_profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <change_entity_map.hpp>
#include <summary_output_stream.hpp>

#include <summary_t.hpp>
#include <identifier_summary_t.hpp>
#include <replacement_summary_t.hpp>
#include <move_summary_t.hpp>
#include <interchange_summary_t.hpp>
#include <expr_stmt_summary_t.hpp>
#include <decl_stmt_summary_t.hpp>
#include <conditional_summary_t.hpp>
#include <jump_summary_t.hpp>

#include <cstdlib>
#include <vector>
#include <memory>
#include <map>

class text_summary {

public:

    enum abstraction_level { HIGH, MEDIUM, LOW };

private:

protected:

        const change_entity_map<parameter_profile_t> & parameters;
        const change_entity_map<call_profile_t>      & member_initializations;

private:

    std::string get_article(const std::string & type_name) const;

    summary_output_stream & summary_dispatch(summary_output_stream & out, const summary_t & summary);

    summary_output_stream & identifier(summary_output_stream & out, const identifier_summary_t & summary) const;
    summary_output_stream & replacement(summary_output_stream & out, const replacement_summary_t & summary) const;
    summary_output_stream & move(summary_output_stream & out, const move_summary_t & summary) const;
    summary_output_stream & interchange(summary_output_stream & out, const interchange_summary_t & summary) const;

    summary_output_stream & expr_stmt(summary_output_stream & out, const expr_stmt_summary_t & summary) const;
    summary_output_stream & decl_stmt(summary_output_stream & out, const decl_stmt_summary_t & summary) const;
    summary_output_stream & conditional(summary_output_stream & out, const conditional_summary_t & summary) const;
    summary_output_stream & jump(summary_output_stream & out, const jump_summary_t & summary) const;

public:

    text_summary(const change_entity_map<parameter_profile_t> & parameters, const change_entity_map<call_profile_t> & member_initializations);

    summary_output_stream & parameter(summary_output_stream & out, size_t number_parameters_deleted,
                            size_t number_parameters_inserted, size_t number_parameters_modified) const;
    summary_output_stream & member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                         size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const;
    summary_output_stream & body(summary_output_stream & out, const std::vector<summary_t> summaries);

};

#endif
