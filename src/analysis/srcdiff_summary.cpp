#include <srcdiff_summary.hpp>

#include <iostream>

srcdiff_summary::srcdiff_summary(const char * xml_encoding) : xml_encoding(xml_encoding) {}
srcdiff_summary::~srcdiff_summary() {}

void srcdiff_summary::summarize(const std::string & srcdiff) {

	srcdiff_summary_handler::profile_list_t profile_list(1024);
	srcdiff_summary_handler handler(profile_list);

	srcSAXController controller(srcdiff, xml_encoding);

	controller.parse(&handler);

	for(srcdiff_summary_handler::profile_t profile : profile_list)
		if(profile.type_name == "function") std::cout << profile << '\n';


}
