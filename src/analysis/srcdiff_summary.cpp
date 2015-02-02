#include <srcdiff_summary.hpp>

#include <srcdiff_summary_handler.hpp>

srcdiff_summary::srcdiff_summary(const char * xml_encoding) : xml_encoding(xml_encoding) {}
srcdiff_summary::~srcdiff_summary() {}

void srcdiff_summary::append(const std::string & srcdiff) {

	srcdiff_summary_handler handler;

	srcSAXController controller(srcdiff, xml_encoding);

	controller.parse(&handler);


}
