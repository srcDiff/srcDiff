#include <shortest_edit_script.hpp>

const int shortest_edit_script::SIZE_THRESHOLD = 2048;
shortest_edit_script::shortest_edit_script(int (*compare)(const void * item_one, const void * item_two, const void * context)
                                       , const void * (*accessor)(int index, const void * structure, const void * context)
                                       , const void * context
                                       , int threshold) : edit_script(nullptr), context(context), compare(compare), accessor(accessor), threshold(threshold) { }

shortest_edit_script::~shortest_edit_script() {

  if(edit_script)
    free_shortest_edit_script(edit_script);

}

int shortest_edit_script::get_size_threshold() {

return SIZE_THRESHOLD;

}

int shortest_edit_script::compute(const void * structure_one, int size_one, const void * structure_two, int size_two) {

    return shortest_edit_script_hybrid(structure_one, size_one, structure_two, size_two, &edit_script, compare, accessor, context, threshold);

}

void shortest_edit_script::set_script(edit * edit_script) {

  this->edit_script = edit_script;

}

edit * shortest_edit_script::get_script() {

  return edit_script;

}
