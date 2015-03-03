#ifndef INCLUDED_SUMMARY_TYPE_HPP
#define INCLUDED_SUMMARY_TYPE_HPP

#include <cstdlib>

bool is_summary_type(size_t summary_types, size_t summary_type_id);

struct summary_type {

public:

	static const size_t NONE  = 0;
	static const size_t TEXT  = 1 << 0;
	static const size_t TABLE = 1 << 1;

};

#endif
