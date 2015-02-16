#ifndef INCLUDED_SRCDIFF_SUMMARY_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HPP

#include <srcdiff_summary_handler.hpp>

#include <string>

class srcdiff_summary {

protected:

private:

	const char * xml_encoding;

public:

	srcdiff_summary(const char * xml_encoding);
	~srcdiff_summary();

	void summarize(const std::string & srcdiff);

};


#endif
