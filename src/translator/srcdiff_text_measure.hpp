#ifndef INCLUDED_SRCDIFF_TEXT_MEASURE_HPP
#define INCLUDED_SRCDIFF_TEXT_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_text_measure : public srcdiff_measure {

protected:

private:

    element_t set_original_text;
    element_t set_modified_text;
    bool important_only;
    bool text_collected;

    void unigrams(element_t & collected_set_original,
                  element_t & collected_set_modified);

public:

	srcdiff_text_measure(const element_t & set_original,
                       const element_t & set_modified,
                       bool important_only = true);

    static void collect_text_node_set(const element_t & set, element_t & set_text);
    void collect_text();
    void collect_important_text();

  	virtual void compute();
    virtual int number_match_beginning();

};





#endif
