#ifndef INCLUDED_SRCDIFF_MATCH_HPP
#define INCLUDED_SRCDIFF_MATCH_HPP

#include <construct.hpp>
#include <srcdiff_measure.hpp>

struct offset_pair {

  int original_offset;
  int original_length;
  int modified_offset;
  int modified_length;
  int similarity;
  offset_pair * next;

  std::pair<int, int> totals() {

    std::pair<int, int> sub_totals = std::make_pair(0, 0);
    for(offset_pair * current = this; current; current = current->next) {

      sub_totals.first += 1;
      sub_totals.second += current->similarity;

    }

    return sub_totals;

  }

};

class srcdiff_match {

public:

  srcdiff_match(const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified);
  offset_pair * match_differences();

protected:

  const construct::construct_list & construct_list_original;
  const construct::construct_list & construct_list_modified;

};

// std::optional<std::string> find_attribute(const std::shared_ptr<const srcML::node> & node, const char * attr_name);

#endif
