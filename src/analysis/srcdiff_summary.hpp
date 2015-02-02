#ifndef INCLUDED_SRCDIFF_SUMMARY_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HPP

#include <srcdiff_summary_handler.hpp>

#include <string>

class srcdiff_summary {

protected:

private:

	const char * xml_encoding;

	srcdiff_summary_handler::class_profile_t class_profile;
	srcdiff_summary_handler::function_profile_t function_profile;

public:

	srcdiff_summary(const char * xml_encoding);
	~srcdiff_summary();

	void append(const std::string & srcdiff);

};


#endif