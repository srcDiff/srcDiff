#ifndef INCLUDED_SRC_TO_NODES_HPP
#define INCLUDED_SRC_TO_NODES_HPP

#include <srcml_nodes.hpp>
#include <node_set.hpp>
#include <memory>

struct node_set_data {

    std::shared_ptr<srcml_nodes> nodes;
    node_set set;

    friend std::ostream & operator<<(std::ostream & out, const node_set_data & data){
        return out << "set: " << data.set;
    }

};

ssize_t str_read(void *, void *, size_t);
int str_close(void *);

std::string read_from_file(std::string);

std::shared_ptr<srcml_nodes> create_nodes(const std::string &, const std::string &);
node_set_data create_node_set(const std::string & code,const std::string & language);

#endif
