// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file shortest_edit_script.hpp
 *
 * @copyright Copyright (C) 2012-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_SHORTEST_EDIT_SCRIPT_HPP
#define INCLUDED_SHORTEST_EDIT_SCRIPT_HPP

#include <shortest_edit_script.h>

#include <vector>
#include <iostream>
#include <cmath>

#include <optional>

class shortest_edit_script_t {
  
protected:

  const static size_t SIZE_THRESHOLD;

  edit_t * edit_script;
  bool approximate;

  const void * context;
  int (*compare)(const void * item_one, const void * item_two, const void * context);
  const void * (*accessor)(int index, const void * structure, const void * context);
  int threshold;

public:

  shortest_edit_script_t(int (*compare)(const void * item_one, const void * item_two, const void * context),
                     const void * (*accessor)(int index, const void * structure, const void * context),
                                        const void * context, int threshold = 1000);

  virtual ~shortest_edit_script_t();

  static size_t get_size_threshold();

  virtual edit_t * script() const;
  virtual void script(edit_t * edit_script);
  bool is_approximate() const;

  template<typename T>
  int approximate_compute(const T & structure_one, const T & structure_two);

  virtual int compute(const void * structure_one, int size_one, const void * structure_two, int size_two);

};

template<typename T>
int shortest_edit_script_t::approximate_compute(const T & structure_one, const T & structure_two) {

  approximate = true;

  const size_t size_one = structure_one.size();
  const size_t size_two = structure_two.size();

  std::vector<std::pair<size_t, size_t>> matches;

  size_t offset_one = 0;
  size_t offset_two = 0;
  while(offset_one < size_one && offset_two < size_two) {

    size_t look_ahead_one = std::min(offset_one + 3, size_one);
    size_t look_ahead_two = std::min(offset_two + 3, size_two);

    std::optional<size_t> match_one;
    std::optional<size_t> match_two;

    for(std::size_t pos_one = offset_one; pos_one < look_ahead_one; ++pos_one) {

      for(std::size_t pos_two = offset_two; pos_two < look_ahead_two; ++pos_two) {

        const void * left = accessor(pos_one, (const void *)structure_one.data(), context);
        const void * right = accessor(pos_two, (const void *)structure_two.data(), context);

        if(compare(left, right, context) != 0) {
          continue;
    }

        match_one = pos_one;
        match_two = pos_two;
        goto end_search;

      }

    }

    end_search:
    if(match_one) {

      offset_one = *match_one + 1;
      offset_two = *match_two + 1;
      matches.push_back(std::make_pair(*match_one, *match_two));

    } else {

      ++offset_one;
      ++offset_two;

    }

  }

  int distance = 0;
  edit_t * last_edit = nullptr;
  size_t last_match_one = 0;
  size_t last_match_two = 0;
  for(const std::pair<size_t, size_t> & pair : matches) {

    size_t delete_length = (std::get<0>(pair) - last_match_one);
    size_t insert_length = (std::get<1>(pair) - last_match_two);

    distance += delete_length + insert_length;

    if(delete_length) {

      edit_t * edit = (struct edit_t *)malloc(sizeof(struct edit_t));
      edit->operation = SES_DELETE;
      edit->offset_sequence_one = last_match_one;
      edit->offset_sequence_two = last_match_two;
      edit->length = delete_length;
      edit->previous = last_edit;
      edit->next = nullptr;

      if(edit_script == nullptr) {
        edit_script = edit;
      }

      if(last_edit) {
        last_edit->next = edit;
      }

      last_edit = edit;

    }

    if(insert_length) {

      edit_t * edit = (struct edit_t *)malloc(sizeof(struct edit_t));
      edit->operation = SES_INSERT;
      edit->offset_sequence_one = last_match_one + delete_length;
      edit->offset_sequence_two = last_match_two;
      edit->length = insert_length;
      edit->previous = last_edit;
      edit->next = nullptr;

      if(edit_script == nullptr) {
        edit_script = edit;
      }

      if(last_edit) {
        last_edit->next = edit;
      }

      last_edit = edit;

    }

    last_match_one = std::get<0>(pair) + 1;
    last_match_two = std::get<1>(pair) + 1;

  }

  size_t delete_length = (size_one - last_match_one);
  size_t insert_length = (size_two - last_match_two);

  distance += delete_length + insert_length;

  if(delete_length) {

    edit_t * edit = (struct edit_t *)malloc(sizeof(struct edit_t));
    edit->operation = SES_DELETE;
    edit->offset_sequence_one = last_match_one;
    edit->offset_sequence_two = last_match_two;
    edit->length = delete_length;
    edit->previous = last_edit;
    edit->next = nullptr;

    if(edit_script == nullptr) {
      edit_script = edit;
    }

    if(last_edit) {
      last_edit->next = edit;
    }

    last_edit = edit;

  }

  if(insert_length) {

    edit_t * edit = (struct edit_t *)malloc(sizeof(struct edit_t));
    edit->operation = SES_INSERT;
    edit->offset_sequence_one = last_match_one + delete_length;
    edit->offset_sequence_two = last_match_two;
    edit->length = insert_length;
    edit->previous = last_edit;
    edit->next = nullptr;

    if(edit_script == nullptr) {
      edit_script = edit;
    }

    if(last_edit) {
      last_edit->next = edit;
    }

    last_edit = edit;

  }

  return distance;

}
#endif
