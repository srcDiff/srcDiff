#ifndef INCLUDED_ELEMENT_LIST_HPP
#define INCLUDED_ELEMENT_LIST_HPP

#include <srcdiff_vector.hpp>

#include <element.hpp>
#include <srcml_nodes.hpp>

#include <srcdiff_macros.hpp>

#include <iostream>

class element_list : public srcdiff_vector<node_set> {

private:

	const srcml_nodes & node_list;

	static bool is_non_white_space(int & node_pos, const srcml_nodes & node_list, const void * context UNUSED);

public:

	typedef std::function<bool (int & node_pos, const srcml_nodes & node_list, const void * context)> node_set_filter;

	element_list(const srcml_nodes & node_list);
	element_list(const element_list & sets);
	element_list(const srcml_nodes & node_list, int start, int end, const node_set_filter & filter = is_non_white_space, const void * context = 0);
	~element_list();

	const srcml_nodes & nodes() const;

	element_list & operator=(element_list sets);
	friend std::ostream & operator<<(std::ostream & out, const element_list & that);

};

#endif
