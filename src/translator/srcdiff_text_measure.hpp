#ifndef INCLUDED_SRCDIFF_TEXT_MEASURE_HPP
#define INCLUDED_SRCDIFF_TEXT_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_text_measure : public srcdiff_measure {

protected:

private:

public:

	srcdiff_text_measure(const node_set & set_original, const node_set & set_modified);

    void compute_ses(class shortest_edit_script & ses);
    void compute_ses_important_text(class shortest_edit_script & ses);

	virtual void compute();

};





#endif
