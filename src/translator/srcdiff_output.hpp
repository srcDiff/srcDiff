#ifndef INCLUDED_SRCDIFFOUTPUT_HPP
#define INCLUDED_SRCDIFFOUTPUT_HPP

#include <srcDiffTypes.hpp>
#include <vector>
#include <pthread.h>
#include <memory>

// const nodes here? or xmlrw

class srcdiff_output {

protected:
	std::shared_ptr<reader_state> rbuf_old;
	std::shared_ptr<reader_state> rbuf_new;
	std::shared_ptr<writer_state> wstate;

	pthread_mutex_t mutex;

private:

public:

    srcdiff_output(const char * srcdiff_filename, METHOD_TYPE method);
    virtual ~srcdiff_output();

    virtual reader_state & get_rbuf_old();
    virtual reader_state & get_rbuf_new();
    virtual writer_state & get_wstate();

	virtual void output_node(const xNodePtr node, int operation);
	virtual void output_text_as_node(const char * text, int operation);
	virtual void output_char(char character, int operation);

	static void update_diff_stack(std::vector<diff_set *> & open_diffs, const xNodePtr node, int operation);

};

#endif
