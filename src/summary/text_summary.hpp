#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <parameter_profile_t.hpp>

#include <summary_t.hpp>
#include <summary_output_stream.hpp>

#include <list>

class text_summary {

protected:

public:

    text_summary();

    summary_output_stream & specifier(summary_output_stream & out, const std::multimap<srcdiff_type, std::string> & specifiers) const;
    summary_output_stream & parameter(summary_output_stream & out, const std::vector<std::shared_ptr<parameter_profile_t>> & parameters) const;
    summary_output_stream & member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                         size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const;
    summary_output_stream & function_body(summary_output_stream & out, std::list<summary_t *> & summaries) const;

};

#endif
