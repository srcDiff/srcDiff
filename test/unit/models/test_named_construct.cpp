// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_named_construct.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE named_construct_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>
#include <named_construct.hpp>

// Define test data
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_named_construct_name = {
    {"class test_class {};"                , "test_class"           , "class"           },
    {"struct test_struct {};"              , "test_struct"          , "struct"          },
    {"union test_union {};"                , "test_union"           , "union"           },
    {"enum test_enum {};"                  , "test_enum"            , "enum"            },
    {"int test_function(){}"               , "test_function"        , "function"        },
    {"int test::test_function(){}"         , "test::test_function"  , "function"        },
    {"void test_function();"               , "test_function"        , "function_decl"   },
    {"class test_class {test_class(){}};"  , "test_class"           , "constructor"     },
    {"class test_class {test_class();};"   , "test_class"           , "constructor_decl"},
    {"class test_class {~test_class(){}};" , "~test_class"          , "destructor"      },
    {"class test_class {~test_class();};"  , "~test_class"          , "destructor_decl" },
    {"int test_decl;"                      , "test_decl"            , "decl"            }
};

BOOST_DATA_TEST_CASE(named_construct_name, data::make(test_cases_named_construct_name), original, expected, construct_type) {

    construct_test_data test_data = create_test_construct(original, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const named_construct> test_named_construct = std::dynamic_pointer_cast<const named_construct>(test_data.test_construct);

    BOOST_TEST(test_named_construct->name()->to_string() == expected);
 }

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_named_construct_is_matchable_impl_true = {
    {"class test_class {};"                , "class test_class {};"                , "class"           },
    {"struct test_struct {};"              , "struct test_struct {};"              , "struct"          },
    {"union test_union {};"                , "union test_union {};"                , "union"           },
    {"enum test_enum {};"                  , "enum test_enum {};"                  , "enum"            },
    {"int test_function(){}"               , "void test_function(){}"              , "function"        },
    {"void test_function();"               , "std::vector<int> test_function();"   , "function_decl"   },
    {"class test_class {test_class(){}};"  , "class test_class {test_class(){}};"  , "constructor"     },
    {"class test_class {test_class();};"   , "class test_class {test_class();};"   , "constructor_decl"},
    {"class test_class {~test_class(){}};" , "class test_class {~test_class(){}};" , "destructor"      },
    {"class test_class {~test_class();};"  , "class test_class {~test_class();};"  , "destructor_decl" },
    {"int test_decl;"                      , "std::string test_decl;"              , "decl"            },
};

BOOST_DATA_TEST_CASE(named_construct_is_matchable_impl_true, data::make(test_cases_named_construct_is_matchable_impl_true), original, modified, construct_type) {

    construct_test_data test_data_original = create_test_construct(original, construct_type);
    BOOST_TEST(test_data_original.test_construct);

    construct_test_data test_data_modified = create_test_construct(modified, construct_type);
    BOOST_TEST(test_data_modified.test_construct);

    std::shared_ptr<const named_construct> test_named_construct_original = std::dynamic_pointer_cast<const named_construct>(test_data_original.test_construct);
    std::shared_ptr<const named_construct> test_named_construct_modified = std::dynamic_pointer_cast<const named_construct>(test_data_modified.test_construct);

    BOOST_TEST(test_named_construct_original->is_matchable_impl(*test_named_construct_modified));
    BOOST_TEST(test_named_construct_modified->is_matchable_impl(*test_named_construct_original));
}

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_named_construct_is_matchable_impl_false = {
    {"class test_class {};"                , "class class_test {};"                , "class"           },
    {"struct test_struct {};"              , "struct struct_test {};"              , "struct"          },
    {"union test_union {};"                , "union union_test {};"                , "union"           },
    {"enum test_enum {};"                  , "enum enum_test {};"                  , "enum"            },
    {"int test_function(){}"               , "void function_test(){}"              , "function"        },
    {"void test_function();"               , "std::vector<int> function_test();"   , "function_decl"   },
    {"class test_class {test_class(){}};"  , "class class_test {class_test(){}};"  , "constructor"     },
    {"class test_class {test_class();};"   , "class class_test {class_test();};"   , "constructor_decl"},
    {"class test_class {~test_class(){}};" , "class class_test {~class_test(){}};" , "destructor"      },
    {"class test_class {~test_class();};"  , "class class_test {~class_test();};"  , "destructor_decl" },
    {"int test_decl;"                      , "std::string decl_test;"              , "decl"            },
};

BOOST_DATA_TEST_CASE(named_construct_is_matchable_impl_false, data::make(test_cases_named_construct_is_matchable_impl_false), original, modified, construct_type) {

    construct_test_data test_data_original = create_test_construct(original, construct_type);
    BOOST_TEST(test_data_original.test_construct);

    construct_test_data test_data_modified = create_test_construct(modified, construct_type);
    BOOST_TEST(test_data_modified.test_construct);

    std::shared_ptr<const named_construct> test_named_construct_original = std::dynamic_pointer_cast<const named_construct>(test_data_original.test_construct);
    std::shared_ptr<const named_construct> test_named_construct_modified = std::dynamic_pointer_cast<const named_construct>(test_data_modified.test_construct);

    BOOST_TEST(!test_named_construct_original->is_matchable_impl(*test_named_construct_modified));
    BOOST_TEST(!test_named_construct_modified->is_matchable_impl(*test_named_construct_original));
}
