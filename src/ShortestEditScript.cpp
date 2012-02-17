#include "ShortestEditScript.hpp"


ShortestEditScript::ShortestEditScript(int (*compare)(void * item_one, void * item_two, void * context)
                                       , void * (*accessor)(int index, void * structure, void * context)
                                       , void * context) : edit_script(NULL), compare(compare), accessor(accessor), context(context) { }

ShortestEditScript::~ShortestEditScript() {

  if(edit_script)
    free_shortest_edit_script(edit_script);

}

ShortestEditScript::compute(int size_one, void * structure_one, int size_two, void * structure_two) {

  shortest_edit_script(size_one, structure_one, size_two, structure_two, compare, accessor, &edit_script, context);

}
