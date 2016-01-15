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

	static bool is_move(const node_set & set);

	static void mark_moves(srcml_nodes & nodes_original,
                           const node_sets & node_sets_original,
                           srcml_nodes & nodes_modified,
                           const node_sets & node_sets_modified,
                           edit * edit_script);

	virtual void output();


};


#endif
