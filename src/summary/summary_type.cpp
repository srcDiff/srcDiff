#include <summary_type.hpp>

bool is_summary_type(size_t summary_types, size_t summary_type_id) {

	return summary_types & summary_type_id;

}
