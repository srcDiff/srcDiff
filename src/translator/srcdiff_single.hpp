#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcdiff_diff.hpp>

#include <srcDiffTypes.hpp>
#include <vector>

class srcdiff_single : public srcdiff_diff {

protected:

	unsigned int start_old;
	unsigned int start_new;

private:
	
	void output_recursive_same();
	void output_recursive_interchangeable();

public:

	srcdiff_single(const srcdiff_diff & diff, unsigned int start_old, unsigned int start_new);

	virtual void output();

};

#endif
