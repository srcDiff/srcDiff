// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_shortest_edit_script.c
 *
 * @copyright Copyright (C) 2014-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE test_shortest_edit_script
#include <boost/test/included/unit_test.hpp>

#include <shortest_edit_script.h>
#include <shortest_edit_script_private.h>

#include <cstring>
#include <vector>

int compare(const void* element_one, const void* element_two, const void* context) {
  
  const char* e1 = (const char*)element_one;
  const char* e2 = (const char*)element_two;

  int match = strcmp(e1, e2);
  return match;
}

const void* accessor(int position, const void* sequence, const void* context) {
  return (((const char**)sequence)[position]);
}

BOOST_AUTO_TEST_SUITE(copy_edit_tests) 

BOOST_AUTO_TEST_CASE(copy_simple) {
  struct edit_t edit{SES_INSERT, 0, 0, 0, nullptr, nullptr};
  struct edit_t* copy = copy_edit(&edit);
  BOOST_TEST(copy);
  BOOST_TEST(copy->operation == SES_INSERT);
  BOOST_TEST(copy->offset_sequence_one == 0);
  BOOST_TEST(copy->offset_sequence_two == 0);
  BOOST_TEST(copy->length == 0);
  BOOST_TEST(copy->next == nullptr);
  BOOST_TEST(copy->previous == nullptr);
  free(copy);
}

BOOST_AUTO_TEST_SUITE_END()