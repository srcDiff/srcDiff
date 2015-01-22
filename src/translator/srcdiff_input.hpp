#ifndef INCLUDED_SRCDIFF_INPUT_HPP
#define INCLUDED_SRCDIFF_INPUT_HPP

#include <srcml_nodes.hpp>
#include <srcdiff_options.hpp>

#include <srcml.h>

#include <vector>
#include <memory>

#include <boost/optional.hpp>

template<class T>
class srcdiff_input {

protected:

	srcml_archive * archive;
	const boost::optional<std::string> input_path;
	const OPTION_TYPE & options;
	const T & input;

private:

public:

	srcdiff_input(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options, const T & input);
	~srcdiff_input();

	void operator()(int stream_source, srcml_nodes & nodes, int & is_input) const;

	virtual srcml_nodes input_nodes(int stream_source) const;

};

#include <srcdiff_input.tcc>

#endif
