#ifndef INCLUDED_SRCDIFF_COMMON_HPP
#define INCLUDED_SRCDIFF_COMMON_HPP

#include <srcdiff_output.hpp>

class srcdiff_common : public srcdiff_output {

protected:

	unsigned int end_old;
	unsigned int end_new;

private:

	static const boost::optional<std::string> whitespace;
	virtual void markup_common();

public:

	srcdiff_common(const srcdiff_output & out, unsigned int end_old, unsigned int end_new);

	virtual void output();

};


#endif
