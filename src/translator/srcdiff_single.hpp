#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcdiff_many.hpp>

class srcdiff_single : public srcdiff_many {

protected:

	unsigned int start_original;
	unsigned int start_modified;

private:

	static const std::string convert;
	
	void output_recursive_same();
	void output_recursive_interchangeable();

public:

	srcdiff_single(const srcdiff_many & diff, unsigned int start_original, unsigned int start_modified);

	virtual void output();

};

#endif
