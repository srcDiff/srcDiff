#ifndef INCLUDED_SRCDIFF_TEXT_MEASURE_HPP
#define INCLUDED_SRCDIFF_TEXT_MEASURE_HPP

#include <srcdiff_measure.hpp>

class srcdiff_text_measure : public srcdiff_measure {

protected:

private:

    construct set_original_text;
    construct set_modified_text;
    bool important_only;
    bool text_collected;

    void unigrams(construct & collected_set_original,
                  construct & collected_set_modified);

public:

	srcdiff_text_measure(const construct & set_original,
                       const construct & set_modified,
                       bool important_only = true);

    static void collect_text_element(const construct & set, construct & set_text);
    void collect_text();
    void collect_important_text();

  	virtual void compute();
    virtual int number_match_beginning();

};





#endif
