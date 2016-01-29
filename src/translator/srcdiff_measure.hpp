#ifndef INCLUDED_SRCDIFF_MEASURE_HPP
#define INCLUDED_SRCDIFF_MEASURE_HPP

#include <node_set.hpp>
#include <shortest_edit_script.hpp>

class srcdiff_measure {

protected:
	const node_set & set_original;
	const node_set & set_modified;

	bool computed;

	int a_similarity;
	int a_difference;
	int original_len;
	int modified_len;

public:

	srcdiff_measure(const node_set & set_original, const node_set & set_modified);

	int similarity()      const;
	int difference()      const;
	int original_length() const;
	int modified_length() const;

	int max_length() const;
	int min_length() const;

	virtual void compute() = 0;

};





#endif
