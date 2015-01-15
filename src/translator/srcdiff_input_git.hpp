#if GIT

#ifndef INCLUDED_SRCDIFF_INPUT_GIT_HPP
#define INCLUDED_SRCDIFF_INPUT_GIT_HPP

#include <srcdiff_input.hpp>

#include <srcdiff_input_source_git.hpp>

class srcdiff_input_git : public srcdiff_input {

protected:

private:

	const srcdiff_input_source_git & git_input;

public:

	srcdiff_input_git(srcml_archive * archive, const boost::optional<std::string> & input_path, const OPTION_TYPE & options, const srcdiff_input_source_git & git_input);
	~srcdiff_input_git();

	virtual srcml_nodes input_nodes(int stream_source) const;

};

#endif

#endif
