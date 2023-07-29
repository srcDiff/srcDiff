#ifndef INCLUDED_CONSTRUCT_LIST_HPP
#define INCLUDED_CONSTRUCT_LIST_HPP

#include <srcdiff_vector.hpp>

#include <construct.hpp>
#include <srcml_nodes.hpp>

#include <srcdiff_macros.hpp>

#include <iostream>

class construct_list : public srcdiff_vector<construct> {

private:

	const srcml_nodes & node_list;

public:

	construct_list(const srcml_nodes & node_list);
	construct_list(const construct_list & sets);
	construct_list(const srcml_nodes & node_list, int start, int end, const construct::construct_filter & filter = construct::is_non_white_space, const void * context = 0);
	~construct_list();

	const srcml_nodes & nodes() const;

	construct_list & operator=(construct_list sets);
	friend std::ostream & operator<<(std::ostream & out, const construct_list & that);

};

#endif
