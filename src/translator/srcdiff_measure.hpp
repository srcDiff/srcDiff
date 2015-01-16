#ifndef INCLUDED_SRCDIFF_MEASURE_HPP
#define INCLUDED_SRCDIFF_MEASURE_HPP

#include <node_set.hpp>
#include <shortest_edit_script.hpp>

#include <vector>

class srcdiff_measure {

protected:
	const srcml_nodes & nodes_original;
	const srcml_nodes & nodes_modified;
	const node_set & set_original;
	const node_set & set_modified;

private:
	void compute_ses(class shortest_edit_script & ses, int & text_original_length, int & text_modified_length);
	void compute_ses_important_text(class shortest_edit_script & ses, int & text_original_length, int & text_modified_length);

public:

	srcdiff_measure(const srcml_nodes & nodes_original, const srcml_nodes & nodes_modified, const node_set & set_original, const node_set & set_modified);

	int compute_similarity();
	int compute_similarity(int & text_original_length, int & text_modified_length);
	void compute_measures(int & similarity, int & difference, int & text_original_length, int & text_modified_length);
	void compute_syntax_measures(int & similarity, int & difference, int & children_original_length, int & children_modified_length);

};





#endif
