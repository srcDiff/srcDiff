#ifndef INCLUDED_SRCDIFF_CHANGE_HPP
#define INCLUDED_SRCDIFF_CHANGE_HPP

#include <srcDiffTypes.hpp>

class srcdiff_change {

protected:
 
	reader_state & rbuf_old;
  	reader_state & rbuf_new;
  	writer_state & wstate;

	int end_old;
	int end_new;

private:

public:

	srcdiff_change(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate, unsigned int end_old, unsigned int end_new);

	virtual void output_whitespace();
	virtual void output();

};

#endif
