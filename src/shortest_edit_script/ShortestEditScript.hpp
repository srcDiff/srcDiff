#ifndef INCLUDED_SHORTESTEDITSCRIPT_HPP
#define INCLUDED_SHORTESTEDITSCRIPT_HPP

#include "shortest_edit_script.h"

class ShortestEditScript {
private:

  edit * edit_script;
  const void * context;
  int (*compare)(const void * item_one, const void * item_two, const void * context);
  const void * (*accessor)(int index, const void * structure, const void * context);


public:

  ShortestEditScript(int (*compare)(const void * item_one, const void * item_two, const void * context),
                     const void * (*accessor)(int index, const void * structure, const void * context),
                                        const void * context);

  ~ShortestEditScript();

  edit * get_script();

  int compute(int size_one, const void * structure_one, int size_two, const void * structure_two);

  //void reset();

};

#endif
