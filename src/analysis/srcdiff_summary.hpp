#ifndef INCLUDED_SRCDIFF_SUMMARY_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HPP

#include <srcdiff_summary_handler.hpp>

#include <string>

class srcdiff_summary {

protected:

private:

	const char * xml_encoding;

	srcdiff_summary_handler::class_profiles_t class_profiles;
	srcdiff_summary_handler::function_profiles_t function_profiles;

public:

	srcdiff_summary(const char * xml_encoding);
	~srcdiff_summary();

	void append(const std::string & srcdiff);

};


#endif