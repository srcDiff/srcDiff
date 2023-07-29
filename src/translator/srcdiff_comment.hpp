#ifndef INCLUDED_SRCDIFF_COMMENT_HPP
#define INCLUDED_SRCDIFF_COMMENT_HPP

#include <srcdiff_diff.hpp>

class srcdiff_comment : public srcdiff_diff {

protected:

private:

public:

	srcdiff_comment(srcdiff_output & out, const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified);
	virtual void output();

};

#endif

