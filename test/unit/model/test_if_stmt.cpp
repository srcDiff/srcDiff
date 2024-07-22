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

// Define test data
namespace data = boost::unit_test::data;
const std::string construct_type = "if_stmt";

std::vector<std::tuple<std::string, std::string>> test_cases_find_if = {
    {"if (y) {}"                           , "if (y) {}"       },
    {"else if (x) {}"                      , "if (x) {}"       },
    {"if (z>=1) {x=1;}"                    , "if (z>=1) {x=1;}"},
    {"if (z!=1) {x=1;}"                    , "if (z!=1) {x=1;}"},
    {"if (1) {a;}"                         , "if (1) {a;}"     },
    {"if (x) {} else if (y) {}"            , "if (x) {}"       },
    {"if (1) a; else if (2) b;"            , "if (1) a;"       },
    {"if (1) {b;} else if (2) a;"          , "if (1) {b;}"     },
    {"if (1) b; else if (2) {a;}"          , "if (1) b;"       },
    {"if (1) {b;} else if (2) {a;} else {}", "if (1) {b;}"     }
};

BOOST_DATA_TEST_CASE(if_stmt_find_if, data::make(test_cases_find_if), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const if_stmt> if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(test_data.test_construct);
    std::shared_ptr<const if_t> if_child = if_stmt_construct->find_if();

    BOOST_TEST(if_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_find_else = {
    {"if (y) {} else {}"                    , "else {}"            },
    {"if (y) {} else x;"                    , "else x;"            },
    {"else if (x) {} else {}"               , "else {}"            },
    {"if (z>=1) {x=1;} else {test::else()}" , "else {test::else()}"},
    {"if (z!=1) {x=1;} else {int x = 0}"    , "else {int x = 0}"   },
    {"if (1) {a;} else {}"                  , "else {}"            },
    {"if (x) {} else if (y) {} else {t;}"   , "else {t;}"          },
    {"if (1) a; else if (2) b; else a;"     , "else a;"            },
    {"if (1) {b;} else if (2) a; else {t;}" , "else {t;}"          },
    {"if (1) b; else if (2) {a;} else nuts;", "else nuts;"         },
    {"if (1) {b;} else if (2) {a;} else {}" , "else {}"            },
};

BOOST_DATA_TEST_CASE(if_stmt_find_else, data::make(test_cases_find_else), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const if_stmt> if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(test_data.test_construct);
    std::shared_ptr<const else_t> else_child = if_stmt_construct->find_else();

    BOOST_TEST(else_child->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_condition = {
    {"if (x) {}"            , "x"           },
    {"if (x>1) {}"          , "x>1"         },
    {"else if (x<1) {}"     , "x<1"         },
    {"if (else()) {}"       , "else()"      },
    {"if (!test) {}"        , "!test"       },
    {"if (a.out) {}"        , "a.out"       },
    {"if (!x) {y=1}"        , "!x"          }
};

BOOST_DATA_TEST_CASE(if_stmt_condition, data::make(test_cases_condition), code, expected) {

    construct_test_data test_data = create_test_construct(code, construct_type);
    BOOST_TEST(test_data.test_construct);

    std::shared_ptr<const if_stmt> if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(test_data.test_construct);

    std::shared_ptr<const construct> if_stmt_condition = if_stmt_construct->condition();

    BOOST_TEST(if_stmt_condition->to_string() == expected);
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_syntax_similar_impl_true = {
    // USE FOR FALSE CASE {"if (x) {}"   , "if (x) {}"}
    {"if (x) {a;b;}"                                                                     , "if (x) {a;b;}"                                                                     },
    {"if (y) {b;a;}"                                                                     , "if (y) {b;a;}"                                                                     },
    {"if (a.out) {f;g;}"                                                                 , "if (a.out) {f;g;}"                                                                 },
    {"if (b.out) {h;i;}"                                                                 , "if (b.out) {h;i;}"                                                                 },
    {"if (!test) {n;j;}"                                                                 , "if (!best) {n;j;}"                                                                 },
    {"else if (x<1) {j;b;}"                                                              , "else if (x<1) {j;b;}"                                                              },
    {"else {j;k;}"                                                                       , "else {j;k;}"                                                                       },
    {"if (x) {y=1;x=0;} else if (y) {j;k;}"                                              , "if (x) {y=1;x=0;} else if (y) {j;k;}"                                              },
    {"if (a.out) {b.out;c.out;} else if (b.out) {a.out;c.out;} else {a.out;b.out;c.out;}", "if (a.out) {b.out;c.out;} else if (b.out) {a.out;c.out;} else {a.out;b.out;c.out;}"}
};

BOOST_DATA_TEST_CASE(if_stmt_is_syntax_similar_impl_true, data::make(test_cases_is_syntax_similar_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const if_stmt> original_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(original_data.test_construct);
    std::shared_ptr<const if_stmt> modified_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(modified_data.test_construct);

   BOOST_TEST(original_if_stmt_construct->is_syntax_similar_impl(*modified_if_stmt_construct));
   BOOST_TEST(modified_if_stmt_construct->is_syntax_similar_impl(*original_if_stmt_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_syntax_similar_impl_false = {
    {"if (x) {}"             , "if (x) {}"           },
    {"if (y) {j;}"           , "if (y) {j;}"         },
    {"else if (a) {}"        , "else if (a) {}"      },
    {"else {}"               , "else {}"             },
    {"if (a.out) {j;k;}"     , "if (a.out) {a;b;}"   },
    {"if (a.out) {j;k;}"     , "else if (a.out) {}"  },
    {"else {b;k;}"           , "else {l;j;}"         },
    {"if (!test) {try;this;}", "if (!go) {fry;it;}"  }
};

BOOST_DATA_TEST_CASE(if_stmt_is_syntax_similar_impl_false, data::make(test_cases_is_syntax_similar_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const if_stmt> original_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(original_data.test_construct);
    std::shared_ptr<const if_stmt> modified_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(modified_data.test_construct);

   BOOST_TEST(!original_if_stmt_construct->is_syntax_similar_impl(*modified_if_stmt_construct));
   BOOST_TEST(!modified_if_stmt_construct->is_syntax_similar_impl(*original_if_stmt_construct));
}


std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_true = {
    {"if (x) {y=1;}"                        , "if (x) {y=1;}"                       },
    {"if (z) {a=1;}"                        , "if (z) {a=1;}"                       },
    {"if (x>1) {y=1;}"                      , "if (x>1) {y=1;}"                     },
    {"if (x>=1) {y=1;}"                     , "if (x>=1) {y=1;}"                    },
    {"if (y<1) {y=1;}"                      , "if (y<1) {y=1;}"                     },
    {"if (z<=1) {y=1;}"                     , "if (x<=1) {y=1;}"                    },
    {"if (x=1) {y=1;}"                      , "if (x=1) {y=1;}"                     },
    {"if (x!=1) {y=1;}"                     , "if (x!=1) {y=1;}"                    },
    {"if (x=1) {y!=1;}"                     , "if (x=1) {y!=1;}"                    },
    {"if (x>=1) {y=1;}"                     , "if (x>=1) {y=1;}"                    },
    {"if (1) {a;}"                          , "if (1) a;"                           },
    {"else if (1) {}"                       , "else if (1) {}"                      },
    {"if (z>=1) {x=1;} else {test::else()}" , "if (z>=1) {x=1;} else {test::else()}"},
    {"if (1) {b;} else if (2) a; else {t;}" , "if (1) {b;} else if (2) a; else {t;}"},
};

BOOST_DATA_TEST_CASE(if_stmt_is_matchable_impl_true, data::make(test_cases_is_matchable_impl_true), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const if_stmt> original_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(original_data.test_construct);
    std::shared_ptr<const if_stmt> modified_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(modified_data.test_construct);

    BOOST_TEST(original_if_stmt_construct->is_matchable_impl(*modified_if_stmt_construct));
    BOOST_TEST(modified_if_stmt_construct->is_matchable_impl(*original_if_stmt_construct));
}

std::vector<std::tuple<std::string, std::string>> test_cases_is_matchable_impl_false = {
    {"if (y) {x=1;}"                        , "if (x) {x=2;}"                               },
    {"if (x!=1) {y=1;}"                     , "if (x=1) {y!=1;}"                            },
    {"if (x>=1) {y=1;}"                     , "if (x=1) {y>=1;}"                            },
    {"if (x<1) {y=1;}"                      , "if (x=1) {y<1;}"                             },
    {"if (z!=1) {x=1;} else {test::else();}", "if (z>=1) {z=0;} else {test();}"             },
    {"if (1) {b;} else if (2) a; else {t;}" , "if (!test) {d;} else if (!undo) a; else {t;}"},
};

BOOST_DATA_TEST_CASE(if_stmt_matchable_impl_false, data::make(test_cases_is_matchable_impl_false), original, modified) {

    construct_test_data original_data = create_test_construct(original, construct_type);
    BOOST_TEST(original_data.test_construct);

    construct_test_data modified_data = create_test_construct(modified, construct_type);
    BOOST_TEST(modified_data.test_construct);

    std::shared_ptr<const if_stmt> original_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(original_data.test_construct);
    std::shared_ptr<const if_stmt> modified_if_stmt_construct = std::dynamic_pointer_cast<const if_stmt>(modified_data.test_construct);

    BOOST_TEST(!original_if_stmt_construct->is_matchable_impl(*modified_if_stmt_construct));
    BOOST_TEST(!modified_if_stmt_construct->is_matchable_impl(*original_if_stmt_construct));
}