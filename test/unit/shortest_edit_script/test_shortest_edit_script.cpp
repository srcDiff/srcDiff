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

extern "C" {
  #include "shortest_edit_script.h"
  #include "shortest_edit_script_private.h"
}

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

BOOST_AUTO_TEST_CASE(copy_stack_links) {
    struct edit_t next, prev;
    struct edit_t edit = { SES_INSERT, 1, 1, 1, &next, &prev };
    struct edit_t* copy = copy_edit(&edit);
    BOOST_REQUIRE(copy);
    BOOST_TEST(copy->operation == SES_INSERT);
    BOOST_TEST(copy->offset_sequence_one == 1);
    BOOST_TEST(copy->offset_sequence_two == 1);
    BOOST_TEST(copy->length == 1);
    BOOST_TEST(copy->next == &next);
    BOOST_TEST(copy->previous == &prev);
    free(copy);
}

BOOST_AUTO_TEST_CASE(copy_malloc_links) {
    struct edit_t * next = (struct edit_t*)malloc(sizeof(struct edit_t));
    struct edit_t * prev = (struct edit_t*)malloc(sizeof(struct edit_t));
    struct edit_t edit = { SES_DELETE, 3, 12, 48, next, prev };
    struct edit_t* copy = copy_edit(&edit);
    BOOST_REQUIRE(copy);
    BOOST_TEST(copy->operation == SES_DELETE);
    BOOST_TEST(copy->offset_sequence_one == 3);
    BOOST_TEST(copy->offset_sequence_two == 12);
    BOOST_TEST(copy->length == 48);
    BOOST_TEST(copy->next == next);
    BOOST_TEST(copy->previous == prev);
    free(next);
    free(prev);
    free(copy);
}

BOOST_AUTO_TEST_CASE(malloc_with_stack_links) {
    struct edit_t next, prev;
    struct edit_t * edit = (struct edit_t*)malloc(sizeof(struct edit_t));
    edit->operation = SES_INSERT;
    edit->offset_sequence_one = 100;
    edit->offset_sequence_two = 50;
    edit->length = 20;
    edit->next = &next;
    edit->previous = &prev;
    struct edit_t* copy = copy_edit(edit);
    BOOST_REQUIRE(copy);
    BOOST_TEST(copy->operation == SES_INSERT);
    BOOST_TEST(copy->offset_sequence_one == 100);
    BOOST_TEST(copy->offset_sequence_two == 50);
    BOOST_TEST(copy->length == 20);
    BOOST_TEST(copy->next == &next);
    BOOST_TEST(copy->previous == &prev);
    free(edit);
    free(copy);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(make_edit_script_tests) 

BOOST_AUTO_TEST_CASE(null_input) {
  struct edit_t* script = nullptr;
  BOOST_TEST(make_edit_script(nullptr, &script, nullptr) == 0);
  BOOST_TEST(script == nullptr);
}

BOOST_AUTO_TEST_CASE(null_input_with_last) {
    struct edit_t* script = nullptr;
    struct edit_t* last = reinterpret_cast<struct edit_t*>(0xdead);
    BOOST_TEST(make_edit_script(nullptr, &script, &last) == 0);
    BOOST_TEST(script == nullptr);
    BOOST_TEST(last == nullptr);
}

BOOST_AUTO_TEST_CASE(one_edit_stack) {
    struct edit_t start{ SES_INSERT, 200, 101, 50, nullptr, nullptr };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int count = make_edit_script(&start, &script, &last);
    BOOST_TEST(count == 1);
    BOOST_REQUIRE(script);
    BOOST_TEST(script->operation == SES_INSERT);
    BOOST_TEST(script->offset_sequence_two == 100);
    BOOST_TEST(script->length == 1);
    free_shortest_edit_script(script);
}


BOOST_AUTO_TEST_CASE(one_edit_malloc) {
    struct edit_t* start = (struct edit_t*)malloc(sizeof(struct edit_t));
    start->operation = SES_INSERT;
    start->offset_sequence_one = 200;
    start->offset_sequence_two = 101;
    start->length = 50;
    start->next = nullptr;
    start->previous = nullptr;
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(start, &script, &last);
    BOOST_TEST(cnt == 1);
    BOOST_REQUIRE(script);
    BOOST_TEST(script->operation == SES_INSERT);
    BOOST_TEST(script->offset_sequence_one == 200);
    BOOST_TEST(script->offset_sequence_two == 100);
    BOOST_TEST(script->length == 1);
    BOOST_TEST(script->next == nullptr);
    BOOST_TEST(script->previous == nullptr);
    BOOST_TEST(script == last);
    free(start);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_insert_condense) {
    struct edit_t first = { SES_INSERT, 0, 1, 1, nullptr, nullptr };
    struct edit_t start = { SES_INSERT, 0, 1, 1, nullptr, &first };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start, &script, &last);
    BOOST_TEST(cnt == 1);
    BOOST_REQUIRE(script);
    BOOST_TEST(script->operation == SES_INSERT);
    BOOST_TEST(script->offset_sequence_one == 0);
    BOOST_TEST(script->offset_sequence_two == 0);
    BOOST_TEST(script->length == 2);
    BOOST_TEST(script->next == nullptr);
    BOOST_TEST(script->previous == nullptr);
    BOOST_TEST(script == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_insert_no_condense) {
    struct edit_t first = { SES_INSERT, 0, 1, 1, nullptr, nullptr };
    struct edit_t start = { SES_INSERT, 1, 1, 1, nullptr, &first };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_delete_condense) {
    struct edit_t first = { SES_DELETE, 1, 0, 1, nullptr, nullptr };
    struct edit_t start = { SES_DELETE, 2, 0, 1, nullptr, &first };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start, &script, &last);
    BOOST_TEST(cnt == 1);
    BOOST_REQUIRE(script);
    BOOST_TEST(script->operation == SES_DELETE);
    BOOST_TEST(script->offset_sequence_one == 0);
    BOOST_TEST(script->offset_sequence_two == 0);
    BOOST_TEST(script->length == 2);
    BOOST_TEST(script->next == nullptr);
    BOOST_TEST(script->previous == nullptr);
    BOOST_TEST(script == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_delete_no_condense) {
    struct edit_t first = { SES_DELETE, 1, 0, 1, nullptr, nullptr };
    struct edit_t start = { SES_DELETE, 1, 0, 1, nullptr, &first };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_insert_delete_1) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t start_edit = { SES_DELETE, 2, 0, 1, NULL, &first_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_insert_delete_2) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t start_edit = { SES_DELETE, 1, 0, 1, NULL, &first_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_delete_insert_1) {
    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, NULL, NULL };
    struct edit_t start_edit = { SES_INSERT, 0, 1, 1, NULL, &first_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(two_delete_insert_2) {
    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, NULL, NULL };
    struct edit_t start_edit = { SES_INSERT, 1, 1, 1, NULL, &first_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_insert_condense_1) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_INSERT, 0, 1, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 1);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 3);
    BOOST_TEST(e->previous == nullptr);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_insert_condense_2) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_INSERT, 1, 1, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 2);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_insert_condense_3) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 1, 1, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_INSERT, 1, 1, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 2);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_insert_condense_4) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 1, 1, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_INSERT, 0, 1, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 3);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_delete_condense_1) {
    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 2, 0, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_DELETE, 3, 0, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 1);
    auto e = script;
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 3);
    BOOST_TEST(e->previous == nullptr);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_delete_condense_2) {
    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 2, 0, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_DELETE, 2, 0, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 2);
    auto e = script;
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 2);
    BOOST_TEST(e->previous == nullptr);
    e = e->next;
    BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 1);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_delete_condense_3) {
    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 1, 0, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_DELETE, 1, 0, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 3);
    auto e = script;
    for (int i = 0; i < 3; ++i) {
        BOOST_TEST(e->operation == SES_DELETE);
        BOOST_TEST(e->offset_sequence_one == 0);
        BOOST_TEST(e->offset_sequence_two == 0);
        BOOST_TEST(e->length == 1);
        if (i < 2) {
            BOOST_REQUIRE(e->next);
            e = e->next;
        }
    }
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_insert_delete_mixed) {
    struct edit_t first_edit = { SES_INSERT, 0, 1, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_DELETE, 1, 0, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_INSERT, 0, 1, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    struct edit_t* last = nullptr;
    int cnt = make_edit_script(&start_edit, &script, &last);
    BOOST_TEST(cnt == 3);
    auto e = script;
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    e = e->next; BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    e = e->next; BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    BOOST_TEST(e == last);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_CASE(three_delete_insert_no_last) {
    struct edit_t first_edit = { SES_DELETE, 1, 0, 1, NULL, NULL };
    struct edit_t middle_edit = { SES_INSERT, 0, 1, 1, NULL, &first_edit };
    struct edit_t start_edit = { SES_DELETE, 1, 0, 1, NULL, &middle_edit };
    struct edit_t* script = nullptr;
    int cnt = make_edit_script(&start_edit, &script, NULL);
    BOOST_TEST(cnt == 3);
    auto e = script;
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    e = e->next; BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_INSERT);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    e = e->next; BOOST_REQUIRE(e);
    BOOST_TEST(e->operation == SES_DELETE);
    BOOST_TEST(e->offset_sequence_one == 0);
    BOOST_TEST(e->offset_sequence_two == 0);
    BOOST_TEST(e->length == 1);
    BOOST_TEST(e->next == nullptr);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(shortest_edit_script_tests)

#define CHECK_SES(fn_call, expected_op, off1, off2, len) \
    do { \
        BOOST_REQUIRE(script); \
        BOOST_TEST(script->operation == expected_op); \
        BOOST_TEST(script->offset_sequence_one == off1); \
        BOOST_TEST(script->offset_sequence_two == off2); \
        BOOST_TEST(script->length == len); \
    } while (0)

// Test 1: NULL/NULL
BOOST_AUTO_TEST_CASE(ses1_null_null) {
    struct edit_t* script = nullptr;
    int dist = shortest_edit_script(nullptr, 0, nullptr, 0, &script, compare, accessor, nullptr);
    BOOST_TEST(dist == 0);
    BOOST_TEST(script == nullptr);
}

// Test 2: empty/empty
BOOST_AUTO_TEST_CASE(ses2_empty_empty) {
    const char* a[] = {};
    const char* b[] = {};
    struct edit_t* script = nullptr;
    int dist = shortest_edit_script(a, 0, b, 0, &script, compare, accessor, nullptr);
    BOOST_TEST(dist == 0);
    BOOST_TEST(script == nullptr);
}

// Test 3
BOOST_AUTO_TEST_CASE(ses3_insert_at_end) {
    const char* a[] = { "a" };
    const char* b[] = { "a", "b" };
    struct edit_t* script = nullptr;
    int dist = shortest_edit_script(a, 1, b, 2, &script, compare, accessor, nullptr);
    BOOST_TEST(dist == 1);
    CHECK_SES(script, SES_INSERT, 1, 1, 1);
    BOOST_TEST(script->next == nullptr);
    BOOST_TEST(script->previous == nullptr);
    free_shortest_edit_script(script);
}

BOOST_AUTO_TEST_SUITE_END() // shortest_edit_script_classic

BOOST_AUTO_TEST_SUITE(shortest_edit_script_linear_variant)

// Test 1: NULL/NULL
BOOST_AUTO_TEST_CASE(ses1_null_null) {
    struct edit_t* script = nullptr;
    int dist = shortest_edit_script_linear_space(nullptr, 0, nullptr, 0, &script, compare, accessor, nullptr);
    BOOST_TEST(dist == 0);
    BOOST_TEST(script == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(shortest_edit_script_hybrid_variant)

// Test 1: NULL/NULL
BOOST_AUTO_TEST_CASE(ses1_null_null) {
    struct edit_t* script = nullptr;
    int dist = shortest_edit_script_hybrid(nullptr, 0, nullptr, 0, &script, compare, accessor, nullptr, 2);
    BOOST_TEST(dist == 0);
    BOOST_TEST(script == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()

// Helper to run a C API SES function and collect its edits
static std::vector<int> run_and_flatten(
    int (*func)(const void*, int, const void*, int, struct edit_t**,
                int(*)(const void*,const void*,const void*),
                const void*(*)(int,const void*,const void*),
                const void*),
    const std::vector<const char*>& a,
    const std::vector<const char*>& b,
    int& out_dist)
{
    struct edit_t* script = nullptr;
    out_dist = func(
        a.empty() ? nullptr : a.data(), a.size(),
        b.empty() ? nullptr : b.data(), b.size(),
        &script,
        compare, accessor, nullptr);
    std::vector<int> flat;
    for (auto e = script; e; e = e->next) {
        flat.push_back(e->operation);
        flat.push_back(static_cast<int>(e->offset_sequence_one));
        flat.push_back(static_cast<int>(e->offset_sequence_two));
        flat.push_back(static_cast<int>(e->length));
    }
    free_shortest_edit_script(script);
    return flat;
}

BOOST_AUTO_TEST_CASE(empty_sequences) {
    int dist;
    auto ops = run_and_flatten(shortest_edit_script, {}, {}, dist);
    BOOST_TEST(dist == 0);
    BOOST_TEST(ops.empty());
}