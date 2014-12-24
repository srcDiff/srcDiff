#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_HPP
#define INCLUDED_SHORTEST_EDIT_SCRIPT_HPP

#include <shortest_edit_script.h>

class shortest_edit_script {
  
private:

  edit * edit_script;
  const void * context;
  int (*compare)(const void * item_one, const void * item_two, const void * context);
  const void * (*accessor)(int index, const void * structure, const void * context);
  int threshold;

public:

  shortest_edit_script(int (*compare)(const void * item_one, const void * item_two, const void * context),
                     const void * (*accessor)(int index, const void * structure, const void * context),
                                        const void * context, int threshold = 1000);

  ~shortest_edit_script();

  edit * get_script();

  int compute(const void * structure_one, int size_one, const void * structure_two, int size_two);

};

#endif
