#ifndef INCLUDED_SRCDIFF_CHANGE_HPP
#define  INCLUDED_SRCDIFF_CHANGE_HPP

#include <srcdiff_diff.hpp>

class srcdiff_change : public srcdiff_diff {

protected:

	int end_old;
	int end_new;

private:

public:

	srcdiff_change(const srcdiff_diff & diff, unsigned int end_old, unsigned int end_new);

	virtual void output_whitespace();
	virtual void output();

};

#endif
