#ifndef SRC_TO_NODES_HPP
#define SRC_TO_NODES_HPP
#include <srcml.h>
#include <srcml_nodes.hpp>
#include <srcdiff_text_measure.hpp>

struct test_data_t {
    node_set node_set_one;
    node_set node_set_two;
    srcdiff_text_measure measure;
    
    friend std::ostream & operator<<(std::ostream & out, const test_data_t & test_data){
        return out << "original:" << test_data.node_set_one << "modified:" << test_data.node_set_two;
    }

};

ssize_t str_read(void *, void *, size_t);
int str_close(void *);

std::string read_from_file(std::string);
srcml_nodes create_nodes(const std::string &, const std::string &);
srcml_nodes create_nodes_file(const std::string &, const std::string &);

test_data_t create_node_set(const std::string &,const std::string &, const std::string &);
test_data_t create_node_set_code(const std::string &,const std::string &, const std::string &);

#endif
