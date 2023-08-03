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
  typedef std::function<bool (const construct::construct_list & sets_original, int start_pos_original,
                              const construct::construct_list & sets_modified, int start_pos_modified,
                              const srcdiff_measure & measure)> is_match_func;
  static bool is_match_default(const construct::construct_list & sets_original, int start_pos_original,
                               const construct::construct_list & sets_modified, int start_pos_modified,
                               const srcdiff_measure & measure);

protected:

  const construct::construct_list & construct_list_original;
  const construct::construct_list & construct_list_modified;

  const is_match_func & is_match;

private:

public:

  srcdiff_match(const construct::construct_list & construct_list_original, const construct::construct_list & construct_list_modified,
                const is_match_func & is_match = is_match_default);
  offset_pair * match_differences();

  static bool reject_match(const srcdiff_measure & measure,
                           const construct & set_original,
                           const construct & set_modified);
  static bool reject_similarity(const srcdiff_measure & measure,
                                const construct & set_original,
                                const construct & set_modified);

  static bool reject_similarity_match_only(const construct & set_original,
                                           const construct & set_modified);


  static bool is_interchangeable_match(const construct & original_set, const construct & modified_set);

};

std::optional<std::string> find_attribute(const std::shared_ptr<srcml_node> & node, const char * attr_name);

#endif
