#ifndef INCLUDED_SRCDIFF_INPUT_HPP
#define INCLUDED_SRCDIFF_INPUT_HPP

#include <srcml_node.hpp>
#include <srcdiff_options.hpp>

#include <srcml.h>

#include <vector>
#include <memory>

#include <boost/optional.hpp>

class srcdiff_input {

protected:

	srcml_archive * archive;
	const boost::optional<std::string> input_path;
	const OPTION_TYPE & options;

private:

public:

	srcdiff_input(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options);
	~srcdiff_input();

	void operator()(int stream_source, std::vector<std::shared_ptr<srcml_node>> & nodes, int & is_input) const;

	virtual std::vector<std::shared_ptr<srcml_node>> input_nodes(int stream_source) const = 0;

};

#endif
