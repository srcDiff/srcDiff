#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcdiff_diff.hpp>

class srcdiff_single : public srcdiff_diff {

protected:

	unsigned int start_original;
	unsigned int start_modified;

private:

	void output_recursive_same();
	void output_recursive_interchangeable();

public:

	srcdiff_single(const srcdiff_diff & diff, unsigned int start_original, unsigned int start_modified);

	virtual void output();

};

#endif
