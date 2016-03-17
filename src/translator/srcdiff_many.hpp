#ifndef INCLUDED_SRCDIFFMANY_HPP
#define INCLUDED_SRCDIFFMANY_HPP

#include <shortest_edit_script.h>
#include <srcdiff_diff.hpp>

#include <vector>

class srcdiff_many : public srcdiff_diff {

protected:

	edit * edit_script;

private:

	typedef std::pair<int, int> int_pair;
	typedef std::vector<int_pair> int_pairs;
  	typedef std::vector<int_pairs> moves;

 	void output_unmatched(int start_original, int end_original, int start_modified, int end_modified);

	moves determine_operations();
  
public:

	srcdiff_many(const srcdiff_diff & diff, edit * edit_script);
	virtual void output();

};

#endif
