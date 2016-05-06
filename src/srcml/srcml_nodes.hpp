#ifndef INCLUDED_SRCML_NODES_HPP
#define INCLUDED_SRCML_NODES_HPP

#include <srcml_node.hpp>

#include <vector>
#include <memory>

typedef std::vector<std::shared_ptr<srcml_node>> srcml_nodes;

void advance_to_tag(srcml_nodes & nodes,
					unsigned int & pos,
                    xmlElementType type,
                    const std::string & name,
                    const std::string & no_pass_ends);

#endif
