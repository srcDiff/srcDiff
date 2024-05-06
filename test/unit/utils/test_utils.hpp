#ifndef INCLUDED_TEST_UTILS_HPP
#define INCLUDED_TEST_UTILS_HPP

#include <srcml_nodes.hpp>
#include <construct_factory.hpp>
#include <memory>

ssize_t str_read(void *, void *, size_t);
int str_close(void *);

std::string read_from_file(std::string);

std::shared_ptr<srcml_nodes> create_nodes(const std::string &, const std::string &);
std::shared_ptr<construct> create_test_construct(const srcml_nodes & nodes);

#endif
