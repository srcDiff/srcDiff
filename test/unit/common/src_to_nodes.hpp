#ifndef INCLUDED_SRC_TO_NODES_HPP
#define INCLUDED_SRC_TO_NODES_HPP

#include <srcml.h>
#include <srcml_nodes.hpp>
#include <srcdiff_text_measure.hpp>


ssize_t str_read(void *, void *, size_t);
int str_close(void *);

std::string read_from_file(std::string);

srcml_nodes create_nodes(const std::string &, const std::string &);

node_set create_node_set(const std::string & code,const std::string & language);

#endif
