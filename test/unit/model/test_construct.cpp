// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_construct.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE construct_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <construct.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "try";

// equals tests true

std::vector<std::tuple<std::string, std::string>> test_cases_operator_equals_true = {
    {"try {int test = 15;}" , "try {int test = 15;}" },
    {"try {}"               , "try {}"               },
    {"try {test();} catch{}", "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_equals_true, data::make(test_cases_operator_equals_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(*original_construct == *modified_construct);
}

// equals tests false

std::vector<std::tuple<std::string, std::string>> test_cases_operator_equals_false = {
    {"try {int test = 15;}" , "try {}"               },
    {"try {}"               , "try {test();}"        },
    {"try {} catch{}"       , "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_equals_false, data::make(test_cases_operator_equals_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(!(*original_construct == *modified_construct));
}

// !equals tests true

std::vector<std::tuple<std::string, std::string>> test_cases_operator_not_equals_true = {
    {"try {test();}"        , "try {}"               },
    {"try {int test = 15;}" , "try {}"               },
    {"try {}"               , "try {test();}"        },
    {"try {} catch{}"       , "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_not_equals_true, data::make(test_cases_operator_not_equals_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(*original_construct != *modified_construct);
}

// !equals tests false

std::vector<std::tuple<std::string, std::string>> test_cases_operator_not_equals_false = {
    {"try {int test = 15;}" , "try {int test = 15;}" },
    {"try {}"               , "try {}"               },
    {"try {test();} catch{}", "try {test();} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_not_equals_false, data::make(test_cases_operator_not_equals_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const construct> original_construct = std::dynamic_pointer_cast<const construct>(original_data.test_construct);
    std::shared_ptr<const construct> modified_construct = std::dynamic_pointer_cast<const construct>(modified_data.test_construct);

    BOOST_TEST(!(*original_construct != *modified_construct));
}

// to_string() tests

std::vector<std::tuple<std::string, std::string>> test_cases_to_string = {
    {"try {}"        , "try {}"        },
    {"try {} catch{}", "try {} catch{}"},
};

BOOST_DATA_TEST_CASE(construct_to_string, data::make(test_cases_to_string), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_CHECK_EQUAL(test_construct->to_string(), expected);
}

// term() tests

std::vector<std::tuple<std::string, std::size_t>> test_cases_term = {
    {"try {}"        , 0},
    {"try{}catch{}", 2},
};

BOOST_DATA_TEST_CASE(construct_term, data::make(test_cases_term), code, pos) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<srcml_nodes> nodes = create_nodes(code, "C++");

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_TEST(*test_construct->term(pos) == *nodes->at(pos));

}

// start_position() tests

std::vector<std::tuple<std::string>> test_cases_start_position = {
    {"try {}"},
    {"try {} catch{}"},

};

BOOST_DATA_TEST_CASE(construct_start_position, data::make(test_cases_start_position), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_TEST(test_construct->start_position() == 0);
}

// end_position() tests

std::vector<std::tuple<std::string, std::size_t>> test_cases_end_position = {
    {"try {}"          , 9 },
    {"try {} catch {}" , 20},
    {"try {int i = 0;}", 33},
};

BOOST_DATA_TEST_CASE(construct_end_position, data::make(test_cases_end_position), code, count) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_TEST(test_construct->end_position() == count);
}

// size() tests

std::vector<std::tuple<std::string, std::size_t>> test_cases_size = {
    {"try {} catch{}", 18}
};

BOOST_DATA_TEST_CASE(construct_size, data::make(test_cases_size), code, size) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_TEST(test_construct->size() == size);
}

// empty() == tests

std::vector<std::tuple<std::string, bool>> test_cases_empty_true = {
    {"try {} catch {}" , false},
    {"try {}"          , false},
};

BOOST_DATA_TEST_CASE(construct_empty_true, data::make(test_cases_empty_true), code, result) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_TEST(test_construct->empty() == result);
}

// empty() != tests


std::vector<std::tuple<std::string, bool>> test_cases_empty_false = {
    {"try {} catch {}" , true},
    {"try {}"          , true},
};

BOOST_DATA_TEST_CASE(construct_empty_false, data::make(test_cases_empty_false), code, result) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);

    BOOST_TEST(!(test_construct->empty() == result));
}

/*
std::vector<std::tuple<std::string, std::string, std::string>> test_cases_parent = {
    {"int i = 0;", "decl", "decl_stmt"}
};

BOOST_DATA_TEST_CASE(construct_parent, data::make(test_cases_parent), code, construct_type, parent_construct) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const construct> test_construct = std::dynamic_pointer_cast<const construct>(test_data.test_construct);



    BOOST_TEST_MESSAGE("DEREFERENCED PARENT: " << *test_construct->term << "\n");

}
*/
