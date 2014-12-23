#ifndef INCLUDED_SRCDIFF_INPUT_FILENAME_HPP
#define INCLUDED_SRCDIFF_INPUT_FILENAME_HPP

#include <srcdiff_input.hpp>

#include <srcml.h>
#include <Options.hpp>
#include <xmlrw.hpp>

#include <vector>

class srcdiff_input_filename : public srcdiff_input {

protected:

private:

public:

	srcdiff_input_filename(srcml_archive * archive, OPTION_TYPE options);
	~srcdiff_input_filename();

	virtual std::vector<xNodePtr> input_nodes(const char * input_path, int stream_source);

};

#endif