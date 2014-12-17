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

public:

	// diff nodes
	std::shared_ptr<xNode> diff_common_start;
	std::shared_ptr<xNode> diff_common_end;
	std::shared_ptr<xNode> diff_old_start;
	std::shared_ptr<xNode> diff_old_end;
	std::shared_ptr<xNode> diff_new_start;
	std::shared_ptr<xNode> diff_new_end;

	std::shared_ptr<xNs> diff;

	// diff attribute
	std::shared_ptr<xAttr> diff_type;

	std::shared_ptr<xNode> unit_tag;

private:

public:

    srcdiff_output(const char * srcdiff_filename, METHOD_TYPE method, const char * prefix);
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
