#ifndef INCLUDED_SRCDIFF_MEASURE_HPP
#define INCLUDED_SRCDIFF_MEASURE_HPP

#include <node_set.hpp>
#include <shortest_edit_script.hpp>

class srcdiff_measure {

protected:
	const srcml_nodes & nodes_original;
	const srcml_nodes & nodes_modified;
	const node_set & set_original;
	const node_set & set_modified;

	bool computed;

	int a_similarity;
	int a_difference;
	int original_len;
	int modified_len;

	void compute_ses(class shortest_edit_script & ses);
	void compute_ses_important_text(class shortest_edit_script & ses);

public:

	srcdiff_measure(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_set & set_original, const node_set & set_modified);

	int similarity()      const;
	int difference()      const;
	int original_length() const;
	int modified_length() const;

	virtual void compute() = 0;

};





#endif
