#ifndef INCLUDED_SHORTESTEDITSCRIPT_HPP
#define INCLUDED_SHORTESTEDITSCRIPT_HPP

#include "shortest_edit_script.hpp"

class ShortestEditScript {
private:

  edit * edit_script;
  void * context;
  int (*compare)(void * item_one, void * item_two, void * context);
  void * (*accessor)(int index, void * structure, void * context);


public:

  ShortestEditScript(int (*compare)(void * item_one, void * item_two, void * context),
                     void * (*accessor)(int index, void * structure, void * context),
                                        void * context);

  ~ShortestEditScript();

  int compute(int size_one, void * structure_one, int size_two, void * structure_two);

};

#endif
