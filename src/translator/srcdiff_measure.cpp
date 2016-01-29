#include <srcdiff_measure.hpp>

#include <algorithm>
#include <cassert>

srcdiff_measure::srcdiff_measure(const node_set & set_original,
                                 const node_set & set_modified) 
  : set_original(set_original),
    set_modified(set_modified),
    computed(false),
    a_similarity(0),
    a_difference(0),
    original_len(0),
    modified_len(0) {}

int srcdiff_measure::similarity() const {

  assert(computed);

  return a_similarity;

}

int srcdiff_measure::difference() const {

  assert(computed);

  return a_difference;

}

int srcdiff_measure::original_length() const {

  assert(computed);

  return original_len;

}

int srcdiff_measure::modified_length() const {

  assert(computed);

  return modified_len;

}

int srcdiff_measure::max_length() const {

  assert(computed);

  return std::max(original_len, modified_len);

}

int srcdiff_measure::min_length() const {

  assert(computed);

  return std::min(original_len, modified_len);

}
