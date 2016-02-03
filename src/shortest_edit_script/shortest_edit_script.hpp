#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_HPP
#define INCLUDED_SHORTEST_EDIT_SCRIPT_HPP

#include <shortest_edit_script.h>

#include <vector>
#include <cmath>

class shortest_edit_script {
  
private:

  edit * edit_script;
  const void * context;
  int (*compare)(const void * item_one, const void * item_two, const void * context);
  const void * (*accessor)(int index, const void * structure, const void * context);
  int threshold;

public:

  static const int SIZE_THRESHOLD = 500;

  shortest_edit_script(int (*compare)(const void * item_one, const void * item_two, const void * context),
                     const void * (*accessor)(int index, const void * structure, const void * context),
                                        const void * context, int threshold = 1000);

  ~shortest_edit_script();

  edit * get_script();

  template<typename T>
  int compute(std::vector<T> structure_one, std::vector<T> structure_two);
  int compute(const void * structure_one, int size_one, const void * structure_two, int size_two);

};

template<typename T>
int shortest_edit_script::compute(std::vector<T> structure_one, std::vector<T> structure_two) {

    const int size_one = structure_one.size();
    const int size_two = structure_two.size();

    if(size_one > SIZE_THRESHOLD || size_two > SIZE_THRESHOLD) {

        int distance = 0;

        int blocks_one = ceil(size_one / SIZE_THRESHOLD);
        int blocks_two = ceil(size_two / SIZE_THRESHOLD);
        int num_blocks = std::max(blocks_one, blocks_two);

        int i = 0;
        edit * last_edit = nullptr;
        for(; i < num_blocks; ++i) {

          edit * edits = nullptr;
          int current_size_one = 0, offset_one = 0;
          if(i < blocks_one) {

            current_size_one = std::min(SIZE_THRESHOLD, size_one - SIZE_THRESHOLD * i);
            offset_one = SIZE_THRESHOLD * i;

          }

          int current_size_two = 0, offset_two = 0;
          if(i < blocks_two) {

            current_size_two = std::min(SIZE_THRESHOLD, size_two - SIZE_THRESHOLD * i);
            offset_two = SIZE_THRESHOLD * i;

          }

          distance += shortest_edit_script_hybrid((const void *)(structure_one.data() + offset_one), size_one, (const void *)(structure_two.data() + offset_two), size_two, &edits, compare, accessor, context, threshold);        

          if(edit_script == nullptr) {

            edit_script = edits;
            for(last_edit = edit_script; last_edit->next != nullptr; last_edit = last_edit->next)
              ;

          }
          else {

            for(last_edit->next = edits; last_edit->next != nullptr; last_edit = last_edit->next)
              ;

          }


        }

        return distance;

    } else {

      return shortest_edit_script_hybrid((const void *)structure_one.data(), size_one, (const void *)structure_two.data(), size_two, &edit_script, compare, accessor, context, threshold);

    }

}

#endif
