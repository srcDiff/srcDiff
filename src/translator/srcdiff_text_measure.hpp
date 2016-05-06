#ifndef INCLUDED_SRCDIFF_TEXT_MEASURE_HPP
#define INCLUDED_SRCDIFF_TEXT_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_text_measure : public srcdiff_measure {

protected:

private:

    node_set set_original_text;
    node_set set_modified_text;
    bool important_only;
    bool text_collected;

    void unigrams(node_set & collected_set_original,
                  node_set & collected_set_modified);

public:

	srcdiff_text_measure(const node_set & set_original,
                       const node_set & set_modified,
                       bool important_only = true);

    static void collect_text_node_set(const node_set & set, node_set & set_text);
    void collect_text();
    void collect_important_text();

  	virtual void compute();
    virtual int number_match_beginning();

};





#endif
