#ifndef INCLUDED_SRCDIFF_INPUT_FILENAME_HPP
#define INCLUDED_SRCDIFF_INPUT_FILENAME_HPP

#include <srcdiff_input.hpp>

class srcdiff_input_filename : public srcdiff_input {

protected:

private:

public:

	srcdiff_input_filename(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options);
	~srcdiff_input_filename();

	virtual srcml_nodes input_nodes(int stream_source) const;

};

#endif
