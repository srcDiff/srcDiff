// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_condition.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#define BOOST_TEST_MODULE conditional_Tests
#include <boost/test/included/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

#include <test_utils.hpp>

#include <conditional.hpp>

// Define test data
namespace data = boost::unit_test::data;

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_condition = {
    {"if (i < 5){};"             , "i < 5"         , "if" },
    {"if (i<0){}"                , "i<0"           , "if" },
    {"if (i=0){}"                , "i=0"           , "if" },
    {"if (i>0){}"                , "i>0"           , "if" },
    {"if (i!=0){}"               , "i!=0"          , "if" },
    {"if (test){}"               , "test"          , "if" },
    {"if (!test){}"              , "!test"         , "if" },
    {"if (test && pass){}"       , "test && pass"  , "if" },
    {"if (*test)"                , "*test"         , "if" },
    {"if (*test && !pass){}"     , "*test && !pass", "if" },
    {"if (!test || !pass){}"     , "!test || !pass", "if" },
    {"else if (i<0){}"           , "i<0"           , "if" },
    {"else if (i=0){}"           , "i=0"           , "if" },
    {"else if (i>0){}"           , "i>0"           , "if" },
    {"else if (i!=0){}"          , "i!=0"          , "if" },
    {"else if (test){}"          , "test"          , "if" },
    {"else if (!test){}"         , "!test"         , "if" },
    {"else if (test && pass){}"  , "test && pass"  , "if" },
    {"else if (*test)"           , "*test"         , "if" },
    {"else if (*test && !pass){}", "*test && !pass", "if" },
    {"else if (!test || !pass){}", "!test || !pass", "if" },
    {"for (int i=0;i<1;i++){}"   , "i<1;"          , "for"},
    {"for (int j=0;j>10;j++){}"  , "j>10;"         , "for"},
    {"for (int k=0;k!=0;k++){}"  , "k!=0;"         , "for"},
    {"for (int l=0;l>=1;l++){}"  , "l>=1;"         , "for"},
    {"for (int m=5;m<=0;m--){}"  , "m<=0;"         , "for"},
    {"for (int n=0;n==0;n++){}"  , "n==0;"         , "for"},
    {"for (;o>5;){}"             , "o>5;"          , "for"}
};

BOOST_DATA_TEST_CASE(conditional_condition, data::make(test_cases_condition), code, expected, construct_type) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const conditional> conditional_construct = std::dynamic_pointer_cast<const conditional>(test_data.test_construct);
    std::shared_ptr<const construct> condition_child = conditional_construct->condition();

    BOOST_TEST(condition_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string, std::string>> test_cases_is_matchable_impl_true = {
    {"if (x) {y=1;}"             , "if (x) {y=1;}"             , "if" },
    {"if (z) {a=1;}"             , "if (z) {a=1;}"             , "if" },
    {"if (x>1) {y=1;}"           , "if (x>1) {y=1;}"           , "if" },
    {"if (x>=1) {y=1;}"          , "if (x>=1) {y=1;}"          , "if" },
    {"if (y<1) {y=1;}"           , "if (y<1) {y=1;}"           , "if" },
    {"if (z<=1) {y=1;}"          , "if (x<=1) {y=1;}"          , "if" },
    {"if (x=1) {y=1;}"           , "if (x=1) {y=1;}"           , "if" },
    {"if (x!=1) {y=1;}"          , "if (x!=1) {y=1;}"          , "if" },
    {"if (x=1) {y!=1;}"          , "if (x=1) {y!=1;}"          , "if" },
    {"if (x>=1) {y=1;}"          , "if (x>=1) {y=1;}"          , "if" },
    {"if (1) {a;}"               , "if (1) a;"                 , "if" },
    {"if (i < 5){};"             , "if (i < 5){};"             , "if" },
    {"if (i<0){}"                , "if (i<0){}"                , "if" },
    {"if (i=0){}"                , "if (i=0){}"                , "if" },
    {"if (i>0){}"                , "if (i>0){}"                , "if" },
    {"if (i!=0){}"               , "if (i!=0){}"               , "if" },
    {"if (test){}"               , "if (test){}"               , "if" },
    {"if (!test){}"              , "if (!test){}"              , "if" },
    {"if (test && pass){}"       , "if (test && pass){}"       , "if" },
    {"if (*test)"                , "if (*test)"                , "if" },
    {"if (*test && !pass){}"     , "if (*test && !pass){}"     , "if" },
    {"if (!test || !pass){}"     , "if (!test || !pass){}"     , "if" },
    {"else if (i<0){}"           , "else if (i<0){}"           , "if" },
    {"else if (i=0){}"           , "else if (i=0){}"           , "if" },
    {"else if (i>0){}"           , "else if (i>0){}"           , "if" },
    {"else if (i!=0){}"          , "else if (i!=0){}"          , "if" },
    {"else if (test){}"          , "else if (test){}"          , "if" },
    {"else if (!test){}"         , "else if (!test){}"         , "if" },
    {"else if (test && pass){}"  , "else if (test && pass){}"  , "if" },
    {"else if (*test)"           , "else if (*test){}"         , "if" },
    {"else if (*test && !pass){}", "else if (*test && !pass){}", "if" },
    {"else if (!test || !pass){}", "else if (!test || !pass){}", "if" },
    {"for (int i=0;i<1;i++){}"   , "for (int i=0;i<1;i++){}"   , "for"},
    {"for (int j=0;j>10;j++){}"  , "for (int j=0;j>10;j++){}"  , "for"},
    {"for (int k=0;k!=0;k++){}"  , "for (int k=0;k!=0;k++){}"  , "for"},
    {"for (int l=0;l>=1;l++){}"  , "for (int l=0;l>=1;l++){}"  , "for"},
    {"for (int m=5;m<=0;m--){}"  , "for (int m=5;m<=0;m--){}"  , "for"},
    {"for (int n=0;n==0;n++){}"  , "for (int n=0;n==0;n++){}"  , "for"},
    {"for (;o>5;){}"             , "for (;o>5;){}"             , "for"}
};

BOOST_DATA_TEST_CASE(conditional_is_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified, construct_type) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const conditional> original_conditional_construct = std::dynamic_pointer_cast<const conditional>(original_data.test_construct);
    std::shared_ptr<const conditional> modified_conditional_construct = std::dynamic_pointer_cast<const conditional>(modified_data.test_construct);

    BOOST_TEST(original_conditional_construct->is_matchable_impl(*modified_conditional_construct));
    BOOST_TEST(modified_conditional_construct->is_matchable_impl(*original_conditional_construct));
}


std::vector<std::tuple<std::string, std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"if (x) {y=1;}"             , "if (z) {y=1;}"             , "if" },
    {"if (z) {a=1;}"             , "if (x) {a=1;}"             , "if" },
    {"if (x>1) {y=1;}"           , "if (j<1) {y=1;}"           , "if" },
    {"if (x>=1) {y=1;}"          , "if (p!=1) {y=1;}"          , "if" },
    {"if (y<1) {y=1;}"           , "if (f>1) {y=1;}"           , "if" },
    {"if (z<=1) {y=1;}"          , "if (d==1) {y=1;}"          , "if" },
    {"if (x=1) {y=1;}"           , "if (f>1) {y=1;}"           , "if" },
    {"if (x!=1) {y=1;}"          , "if (b>=1) {y=1;}"          , "if" },
    {"if (x=1) {y!=1;}"          , "if (s<1) {y!=1;}"          , "if" },
    {"if (x>=1) {y=1;}"          , "if (l!=1) {y=1;}"          , "if" },
    {"if (1) {a;}"               , "if (4) a;"                 , "if" },
    {"if (i < 5){};"             , "if (j > 5){};"             , "if" },
    {"if (i<0){}"                , "if (y>0){}"                , "if" },
    {"if (i=0){}"                , "if (k!=0){}"               , "if" },
    {"if (i>0){}"                , "if (g<0){}"                , "if" },
    {"if (i!=0){}"               , "if (h=0){}"                , "if" },
    {"if (test){}"               , "if (!test){}"              , "if" },
    {"if (!test){}"              , "if (test){}"               , "if" },
    {"if (test && pass){}"       , "if (test || pass){}"       , "if" },
    {"if (*test)"                , "if (&test == 0)"           , "if" },
    {"if (*test && !pass){}"     , "if (*pass || !pass){}"     , "if" },
    {"if (!test || !pass){}"     , "if (ktest && *test){}"     , "if" },
    {"else if (i<0){}"           , "else if (i>=0){}"          , "if" },
    {"else if (i=0){}"           , "else if (jd=2){}"          , "if" },
    {"else if (i>0){}"           , "else if (k<=0){}"          , "if" },
    {"else if (i!=0){}"          , "else if (l=0){}"           , "if" },
    {"else if (test){}"          , "else if (!it){}"           , "if" },
    {"else if (!test){}"         , "else if (*test){}"         , "if" },
    {"else if (test && pass){}"  , "else if (pass && fail){}"  , "if" },
    {"else if (*test)"           , "else if (tester){}"        , "if" },
    {"else if (*test && !pass){}", "else if (*pass || *pass){}", "if" },
    {"else if (!test || !pass){}", "else if (test || pass){}"  , "if" },
    {"for (int i=0;i<1;i++){}"   , "for (int i=0;l>=25;i++){}" , "for"},
    {"for (int j=0;j>10;j++){}"  , "for (int j=0;l=1;j++){}"   , "for"},
    {"for (int k=0;k!=0;k++){}"  , "for (int k=0;p>=t;k++){}"  , "for"},
    {"for (int l=0;l>=1;l++){}"  , "for (int l=0;h<=1;l++){}"  , "for"},
    {"for (int m=5;m<=0;m--){}"  , "for (int m=5;e<=1;m--){}"  , "for"},
    {"for (int n=0;n==0;n++){}"  , "for (int n=0;k==f;n++){}"  , "for"},
    {"for (;o>5;){}"             , "for (;0>5;){}"             , "for"}
};

BOOST_DATA_TEST_CASE(conditional_is_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified, construct_type) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const conditional> original_conditional_construct = std::dynamic_pointer_cast<const conditional>(original_data.test_construct);
    std::shared_ptr<const conditional> modified_conditional_construct = std::dynamic_pointer_cast<const conditional>(modified_data.test_construct);

    BOOST_TEST(!original_conditional_construct->is_matchable_impl(*modified_conditional_construct));
    BOOST_TEST(!modified_conditional_construct->is_matchable_impl(*original_conditional_construct));
}
