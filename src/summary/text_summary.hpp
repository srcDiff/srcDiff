#ifndef INCLUDED_TEXT_SUMMARY_HPP
#define INCLUDED_TEXT_SUMMARY_HPP

#include <summary_t.hpp>
#include <summary_output_stream.hpp>

#include <list>

class text_summary {

protected:

public:

    text_summary();

    summary_output_stream & parameter(summary_output_stream & out, size_t number_parameters_deleted,
                            size_t number_parameters_inserted, size_t number_parameters_modified) const;
    summary_output_stream & member_initialization(summary_output_stream & out, size_t number_member_initializations_deleted,
                                         size_t number_member_initializations_inserted, size_t number_member_initializations_modified) const;
    summary_output_stream & function_body(summary_output_stream & out, std::list<summary_t *> & summaries) const;

};

#endif
