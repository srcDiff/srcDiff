#ifndef INCLUDED_SRCDIFF_CHANGE_HPP
#define INCLUDED_SRCDIFF_CHANGE_HPP

#include <srcdiff_output.hpp>

class srcdiff_change : public srcdiff_output {

protected:
 
	int end_old;
	int end_new;

private:

	static const boost::optional<std::string> change;

public:

	srcdiff_change(const srcdiff_output & out, unsigned int end_old, unsigned int end_new);

	virtual void output_whitespace();
	virtual void output();

};

#endif
