#include "ShortestEditScript.hpp"


ShortestEditScript::ShortestEditScript(int (*compare)(void * structure_one, void * structure_two, void * context)
                                       , void * (*accessor)(int index, void * structure, void * context)
                                       , void * context) {

}
