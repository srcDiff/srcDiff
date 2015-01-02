#if SVN

#ifndef INCLUDED_SRCDIFF_INPUT_SVN_HPP
#define INCLUDED_SRCDIFF_INPUT_SVN_HPP

#include <srcdiff_input.hpp>

#include <srcdiff_input_source_svn.hpp>

class srcdiff_input_svn : public srcdiff_input {

protected:

private:

	const srcdiff_input_source_svn & svn_input;

public:

	srcdiff_input_svn(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options, const srcdiff_input_source_svn & svn_input);
	~srcdiff_input_svn();

	virtual std::vector<xNodePtr> input_nodes(const boost::optional<std::string> & input_path, int stream_source);

};

#endif

#endif
