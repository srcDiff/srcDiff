#include <ShortestEditScript.hpp>


ShortestEditScript::ShortestEditScript(int (*compare)(const void * item_one, const void * item_two, const void * context)
                                       , const void * (*accessor)(int index, const void * structure, const void * context)
                                       , const void * context
                                       , int threshold) : edit_script(0), context(context), compare(compare), accessor(accessor), threshold(threshold) { }

ShortestEditScript::~ShortestEditScript() {

  if(edit_script)
    free_shortest_edit_script(edit_script);

}

int ShortestEditScript::compute(const void * structure_one, int size_one, const void * structure_two, int size_two) {

  return shortest_edit_script_hybrid(structure_one, size_one, structure_two, size_two, &edit_script, compare, accessor, context, threshold);

}

edit * ShortestEditScript::get_script() {

  return edit_script;

}
