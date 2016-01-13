#ifndef INCLUDED_SRCDIFF_TEXT_MEASURE_HPP
#define INCLUDED_SRCDIFF_TEXT_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_text_measure : public srcdiff_measure {

protected:

private:

public:

	srcdiff_text_measure(const node_set & set_original, const node_set & set_modified);

	virtual void compute();

};





#endif
