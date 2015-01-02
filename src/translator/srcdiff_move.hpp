#ifndef INCLUDED_SRCDIFF_MOVE_HPP
#define INCLUDED_SRCDIFF_MOVE_HPP

#include <srcdiff_output.hpp>

#include <node_sets.hpp>
#include <shortest_edit_script.h>

class srcdiff_move : public srcdiff_output {

protected:

	unsigned int & position;
	int operation;

private:

public:

	srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation);

	static bool is_move(node_set * set, const std::vector<xNodePtr> & nodes);

	static void mark_moves(std::vector<xNodePtr> & nodes_old, node_sets * node_sets_old
	                , std::vector<xNodePtr> & nodes_new, node_sets * node_sets_new
	                , edit * edit_script);

	virtual void output();


};


#endif
