#ifndef INCLUDED_SRCDIFFSINGLE_HPP
#define INCLUDED_SRCDIFFSINGLE_HPP

#include <srcdiff_diff.hpp>

#include <srcDiffTypes.hpp>
#include <vector>

class srcdiff_single : public srcdiff_diff {

protected:

	unsigned int start_old;
	unsigned int start_new;

	void output_recursive_same();
	void output_recursive_interchangeable();

public:

	srcdiff_single(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate, node_sets * node_sets_old, node_sets * node_sets_new, unsigned int start_old, unsigned int start_new);


	virtual void output();

};

#endif
