#ifndef INCLUDED_SRCDIFF_TEXT_MEASURE_HPP
#define INCLUDED_SRCDIFF_TEXT_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_text_measure : public srcdiff_measure {

protected:

private:

    node_set set_original_text;
    node_set set_modified_text;

    void unigrams(node_set & collected_set_original,
                  node_set & collected_set_modified);

public:

	srcdiff_text_measure(const node_set & set_original, const node_set & set_modified);

    void collect_text();
    void collect_important_text();

	virtual void compute();

};





#endif
