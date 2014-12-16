#ifndef INCLUDED_SRCDIFFOUTPUT_HPP
#define INCLUDED_SRCDIFFOUTPUT_HPP

#include <srcDiffTypes.hpp>
#include <vector>

class srcdiff_output {

protected:
	reader_state rbuf_old;
	reader_state rbuf_new;
	writer_state wstate;

private:

	static void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation);


public:

    srcdiff_output(reader_state & rbuf_old, reader_state & rbuf_new, writer_state & wstate);

	virtual output_node(const xNodePtr node, int operation);
	virtual output_text_as_node(const char * text, int operation);
	virtual output_char(char character, int operation);


};

#endif
