#ifndef INCLUDED_SRCDIFFMANY_HPP
#define INCLUDED_SRCDIFFMANY_HPP

#include <shortest_edit_script.h>
#include <srcdiff_diff.hpp>

#include <vector>

class srcdiff_many : public srcdiff_diff {

protected:

	edit * edit_script;

private:

	typedef std::pair<int, int> IntPair;
	typedef std::vector<IntPair> IntPairs;
  	typedef std::vector<IntPairs> Moves;

 	void output_unmatched(int start_original, int end_original, int start_modified, int end_modified);

	Moves determine_operations();
  
public:

	srcdiff_many(const srcdiff_diff & diff, edit * edit_script);
	virtual void output();

};

#endif
