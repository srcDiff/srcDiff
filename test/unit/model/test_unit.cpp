// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_unit.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE unit_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <unit.hpp>

#include <test_utils.hpp>

namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string>> unit_test_cases = {
    {"int i = 0;"},
};

BOOST_DATA_TEST_CASE(unit_tests, data::make(unit_test_cases), code) {
    std::shared_ptr<srcml_nodes> nodes = create_nodes(code, "C++");
    BOOST_TEST(nodes->size() >= 1);
}
