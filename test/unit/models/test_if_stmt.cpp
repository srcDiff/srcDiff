
// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_if_stmt.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE if_stmt_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <if_stmt.hpp>

//Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "if_stmt";

std::vector<std::tuple<std::string>> test_cases_find_if = {
    {"if (y) {}"                           },
    {"else if (x) {}"                      },
    {"if (z>=1) {x=1;}"                    },
    {"if (z!=1) {x=1;}"                    },
    {"if (1) {a;}"                         },
    {"if (x) {} else if (y) {}"            },
    {"if (1) a; else if (2) b;"            },
    {"if (1) {b;} else if (2) a;"          },
    {"if (1) b; else if (2) {a;}"          },
    {"if (1) {b;} else if (2) {a;} else {}"},
};

BOOST_DATA_TEST_CASE(if_stmt_find_if, data::make(test_cases_find_if), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const if_stmt> if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(test_data.test_construct);
    BOOST_TEST(if_stmt_construct->find_if());
}

std::vector<std::tuple<std::string>> test_cases_find_else = {
    {"if (y) {} else {}"                    },
    {"if (y) {} else x;"                    },
    {"else if (x) {} else {}"               },
    {"if (z>=1) {x=1;} else {test::else()}" },
    {"if (z!=1) {x=1;} else {int x = 0}"    },
    {"if (1) {a;} else {}"                  },
    {"if (x) {} else if (y) {} else {t;}"   },
    {"if (1) a; else if (2) b; else a;"     },
    {"if (1) {b;} else if (2) a; else {t;}" },
    {"if (1) b; else if (2) {a;} else nuts;"},
    {"if (1) {b;} else if (2) {a;} else {}" },
};

BOOST_DATA_TEST_CASE(if_stmt_find_else, data::make(test_cases_find_else), code) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const if_stmt> if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(test_data.test_construct);
    BOOST_TEST(if_stmt_construct->find_else());
}
