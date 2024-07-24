// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_block_t.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE block_t_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <block.hpp>

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "block";

std::vector<std::tuple<std::string, std::string>> test_cases_block_content = {
    {"{block();}"  , "block();"  },
    {"{a;b;{c;}}"  , "a;b;{c;}"  },
    {"{{}}"        , "{}"        },
    {"{i;{j;{k;}}}", "i;{j;{k;}}"},
    {"{}"          , ""          }
};

BOOST_DATA_TEST_CASE(block_block_content, data::make(test_cases_block_content), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const block> block_construct = std::dynamic_pointer_cast<const block>(test_data.test_construct);
    std::shared_ptr<const construct> block_child = block_construct->block_content();

    BOOST_TEST(block_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_syntax_similar_impl_true = {
    {"{block();}", "{block();}"}
};

BOOST_DATA_TEST_CASE(block_is_syntax_similar_impl_true, data::make(test_cases_is_syntax_similar_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const block> original_block = std::dynamic_pointer_cast<const block>(original_data.test_construct);
    std::shared_ptr<const block> modified_block = std::dynamic_pointer_cast<const block>(modified_data.test_construct);

    BOOST_TEST(original_block->is_syntax_similar_impl(*modified_block));
    BOOST_TEST(modified_block->is_syntax_similar_impl(*original_block));
}
