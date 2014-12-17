#ifndef INCLUDED_SRCDIFF_COMMENT_HPP
#define INCLUDED_SRCDIFF_COMMENT_HPP

#include <srcdiff_diff.hpp>

class srcdiff_comment : public srcdiff_diff {

protected:

private:

public:

	srcdiff_comment(srcdiff_output & out, reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate, node_sets * node_sets_old, node_sets * node_sets_new);
	virtual void output();

};

#endif

