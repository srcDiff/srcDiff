#ifndef INCLUDED_SRCDIFF_SUMMARY_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HPP

#include <srcdiff_summary_handler.hpp>

#include <string>

class srcdiff_summary {

protected:

private:

public:

	srcdiff_summary();
	~srcdiff_summary();

	void summarize(const std::string & srcdiff, const std::string & xml_encoding);

};


#endif
