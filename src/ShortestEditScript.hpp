#ifndef INCLUDED_SHORTESTEDITSCRIPT_HPP
#define INCLUDED_SHORTESTEDITSCRIPT_HPP

#include "shortest_edit_script.hpp"

class ShortestEditScript {
private:

  edit * edit_script;
  void * context;
  void (*accessor)(int index, void * structure, void * context);
  int (*compare)(void * item_one, void * item_two, void * context);

public:

  ~ShortestEditScript();



};

#endif
