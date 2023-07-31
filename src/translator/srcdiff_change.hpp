#ifndef INCLUDED_SRCDIFF_CHANGE_HPP
#define INCLUDED_SRCDIFF_CHANGE_HPP

#include <srcdiff_output.hpp>

class srcdiff_change : public srcdiff_output {

protected:
 
	int end_original;
	int end_modified;

private:

public:

	static void output_change(const srcdiff_output & out, int end_original, int end_modified) {
 	 srcdiff_change change(out, end_original, end_modified);
	 change.output();
	}

	srcdiff_change(const srcdiff_output & out, unsigned int end_original, unsigned int end_modified);

	virtual void output_whitespace_all();
	virtual void output_whitespace_prefix();
	virtual void output();

};

#endif
