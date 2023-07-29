#ifndef INCLUDED_SRCDIFF_MOVE_HPP
#define INCLUDED_SRCDIFF_MOVE_HPP

#include <srcdiff_output.hpp>

#include <construct.hpp>
#include <shortest_edit_script.h>

class srcdiff_move : public srcdiff_output {

protected:

	unsigned int & position;
	int operation;

private:

public:

	srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation);

	static bool is_move(const construct & set);

	static void mark_moves(const construct::construct_list & construct_list_original,
                           const construct::construct_list & construct_list_modified,
                           edit_t * edit_script);

	virtual void output();


};


#endif
