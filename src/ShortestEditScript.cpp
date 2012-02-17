#include "ShortestEditScript.hpp"


ShortestEditScript::ShortestEditScript(int (*compare)(void * item_one, void * item_two, void * context)
                                       , void * (*accessor)(int index, void * structure, void * context)
                                       , void * context) : edit_script(NULL), compare(compare), accessor(accessor), context(context) { }
