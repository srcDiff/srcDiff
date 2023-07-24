#ifndef INCLUDED_SRCDIFF_SYNTAX_MEASURE_HPP
#define INCLUDED_SRCDIFF_SYNTAX_MEASURE_HPP

#include <srcdiff_measure.hpp>

#include <cassert>

class srcdiff_syntax_measure : public srcdiff_measure {

protected:

private:

public:

	srcdiff_syntax_measure(const element_t & set_original, const element_t & set_modified);

	virtual void compute();

};





#endif
