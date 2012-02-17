#include "ShortestEditScript.hpp"


ShortestEditScript::ShortestEditScript(int (*compare)(const void * item_one, const void * item_two, const void * context)
                                       , const void * (*accessor)(int index, const void * structure, const void * context)
                                       , const void * context) : edit_script(0), context(context), compare(compare), accessor(accessor) { }

ShortestEditScript::~ShortestEditScript() {

  if(edit_script)
    free_shortest_edit_script(edit_script);

}

int ShortestEditScript::compute(int size_one, const void * structure_one, int size_two, const void * structure_two) {

  return shortest_edit_script(size_one, structure_one, size_two, structure_two, compare, accessor, &edit_script, context);

}
