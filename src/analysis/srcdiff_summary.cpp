#include <srcdiff_summary.hpp>

#include <summarize_profile.hpp>

#include <iostream>

srcdiff_summary::srcdiff_summary(const char * xml_encoding) : xml_encoding(xml_encoding) {}
srcdiff_summary::~srcdiff_summary() {}

void srcdiff_summary::summarize(const std::string & srcdiff) {

	profile_t::profile_list_t profile_list(1024);
	srcdiff_summary_handler handler(profile_list);

	srcSAXController controller(srcdiff, xml_encoding);

	controller.parse(&handler);

	// should always be unit
	summarize_profile summarizer(profile_list);
	summarizer.summarize(std::cout, 1);


}
