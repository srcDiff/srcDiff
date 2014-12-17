#ifndef INCLUDED_SRCDIFFMOVE_HPP
#define INCLUDED_SRCDIFFMOVE_HPP

#include <srcDiffOutput.hpp>

#include <shortest_edit_script.h>

class srcdiff_move : public srcdiff_output {

protected:

	unsigned int & position;
	int operation;

private:

public:

	srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation);

	static bool is_move(node_set * set, std::vector<xNodePtr> nodes);

	static void mark_moves(reader_state & rbuf_old, node_sets * node_sets_old
	                , reader_state & rbuf_new, node_sets * node_sets_new
	                , edit * edit_script, writer_state & wstate);

	virtual void output();


};


#endif
