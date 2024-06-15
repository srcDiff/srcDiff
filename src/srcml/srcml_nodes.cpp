// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file srcml_nodes.cpp
 *
 * @copyright Copyright (C) 2016-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <srcml_nodes.hpp>
#include <iostream>

//static xmlReaderTypes START_TAG_TYPE = XML_READER_TYPE_ELEMENT;
//static xmlReaderTypes END_TAG_TYPE = XML_READER_TYPE_END_ELEMENT;

void advance_to_child(const srcml_nodes & nodes,
                      size_t & pos,
                      srcML::node_type type,
                      const std::string & name) {

  srcml_nodes::size_type size = nodes.size();

  size_t start_pos = pos;
  int depth = 0;

  while(start_pos < size) {

    if(nodes.at(start_pos)->get_type() == srcML::node_type::END) {
      --depth;
    }

    if(nodes.at(start_pos)->get_type() == type
      && nodes.at(start_pos)->get_name() == name
       && depth == 0) {
      break;
    }

    if(!nodes.at(start_pos)->is_empty() && nodes.at(start_pos)->get_type() == srcML::node_type::START) {
        ++depth;
    }

    if(depth < 0) {

      --start_pos;
      break;

    }

    ++start_pos; 

  }

  pos = start_pos;

}
