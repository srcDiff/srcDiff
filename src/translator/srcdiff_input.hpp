#ifndef INCLUDED_SRCDIFF_INPUT_HPP
#define INCLUDED_SRCDIFF_INPUT_HPP

#include <srcml.h>

class srcdiff_input {

protected:

	srcml_archive * archive;
	OPTION_TYPE options;

private:

public:

	srcdiff_input(srcml_archive * archive, OPTION_TYPE options);
	~srcdiff_input();

	std::vector<xNodePtr> input_nodes(const char * input_path, int stream_source);

};

#endif