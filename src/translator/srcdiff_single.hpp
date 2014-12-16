#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcdiff_many.hpp>

#include <srcDiffTypes.hpp>
#include <vector>

class srcdiff_single : public srcdiff_many {

protected:

	unsigned int start_old;
	unsigned int start_new;

private:
	
	void output_recursive_same();
	void output_recursive_interchangeable();

public:

	srcdiff_single(const srcdiff_many & diff, unsigned int start_old, unsigned int start_new);

	virtual void output();

};

#endif
