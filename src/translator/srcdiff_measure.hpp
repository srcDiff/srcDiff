#ifndef INCLUDED_SRCDIFF_MEASURE_HPP
#define INCLUDED_SRCDIFF_MEASURE_HPP

#include <node_set.hpp>
#include <shortest_edit_script.hpp>

#include <vector>

class srcdiff_measure {

protected:
	const std::vector<xNodePtr> & nodes_old;
	const std::vector<xNodePtr> & nodes_new;
	node_set * set_old;
	node_set * set_new;

private:
	void compute_ses(class shortest_edit_script & ses, int & text_old_length, int & text_new_length);
	void compute_ses_important_text(class shortest_edit_script & ses, int & text_old_length, int & text_new_length);

public:

	srcdiff_measure(const std::vector<xNodePtr> & nodes_old, const std::vector<xNodePtr> & nodes_new, node_set * set_old, node_set * set_new);

	int compute_similarity();
	int compute_similarity(int & text_old_length, int & text_new_length);
	void compute_measures(int & similarity, int & difference, int & text_old_length, int & text_new_length);
	void compute_syntax_measures(int & similarity, int & difference, int & children_old_length, int & children_new_length);

};





#endif
