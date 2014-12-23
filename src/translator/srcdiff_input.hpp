#ifndef INCLUDED_SRCDIFF_INPUT_HPP
#define INCLUDED_SRCDIFF_INPUT_HPP

#include <srcml.h>
#include <Options.hpp>
#include <xmlrw.hpp>

#include <vector>

class srcdiff_input {

protected:

	srcml_archive * archive;
	OPTION_TYPE options;

private:

public:

	srcdiff_input(srcml_archive * archive, OPTION_TYPE options);
	~srcdiff_input();

	void operator()(const char * input_path, int stream_source, std::vector<xNodePtr> & nodes, int & is_input);

	virtual std::vector<xNodePtr> input_nodes(const char * input_path, int stream_source) = 0;

};

#endif