#ifndef INCLUDED_SRCDIFF_MOVE_HPP
#define INCLUDED_SRCDIFF_MOVE_HPP

#include <srcdiff_output.hpp>

#include <element_list.hpp>
#include <shortest_edit_script.h>

class srcdiff_move : public srcdiff_output {

protected:

	unsigned int & position;
	int operation;

private:

public:

	srcdiff_move(const srcdiff_output & out, unsigned int & position, int operation);

	static bool is_move(const element_t & set);

	static void mark_moves(srcml_nodes & nodes_original,
                           const element_list & element_list_original,
                           srcml_nodes & nodes_modified,
                           const element_list & element_list_modified,
                           edit_t * edit_script);

	virtual void output();


};


#endif
