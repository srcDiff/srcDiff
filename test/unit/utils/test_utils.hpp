// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file test_utils.hpp
 *
 * @copyright Copyright (C) 2020-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#ifndef INCLUDED_TEST_UTILS_HPP
#define INCLUDED_TEST_UTILS_HPP

#include <srcml_nodes.hpp>
#include <construct_factory.hpp>
#include <construct.hpp>
#include <unit.hpp>
#include <memory>

ssize_t str_read(void *, void *, size_t);
int str_close(void *);

std::string read_from_file(std::string);

struct construct_test_data {
    std::shared_ptr<srcml_nodes> nodes;
    std::shared_ptr<const construct>   test_construct;

};

std::shared_ptr<srcml_nodes> create_nodes(const std::string &, const std::string &);

std::shared_ptr<const construct>  create_test_construct_inner(std::shared_ptr<construct> construct, size_t pos);

construct_test_data create_test_construct(const std::string & code, const std::string & construct_name, const std::string & language = "C++");

#endif
