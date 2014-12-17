#ifndef INCLUDED_SRCDIFFOUTPUT_HPP
#define INCLUDED_SRCDIFFOUTPUT_HPP

#include <srcDiffTypes.hpp>
#include <vector>

// const nodes here? or xmlrw

class srcdiff_output {

protected:
	reader_state & rbuf_old;
	reader_state & rbuf_new;
	writer_state & wstate;

private:

public:

    srcdiff_output(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate);

	virtual void output_node(const xNodePtr node, int operation);
	virtual void output_text_as_node(const char * text, int operation);
	virtual void output_char(char character, int operation);

	static void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation);

};

#endif
