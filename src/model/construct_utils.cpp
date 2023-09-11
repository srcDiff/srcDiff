/**
 * @file construct_utils.cpp
 *
 * @copyright Copyright (C) 2023-2023 srcML, LLC. (www.srcML.org)
 *
 * srcDiff is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * srcDiff is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <construct_utils.hpp>
#include <construct.hpp>

namespace std {

  size_t hash<construct>::operator()(const construct & element) const {

    size_t result = 2166136261;
    for(std::size_t pos = 0, size = element.size(); pos < size; ++pos) {

        const shared_ptr<srcml_node> & term = element.term(pos);
        const string & hash_item = term->is_text() ? *term->content : term->get_name();
        for(size_t hash_pos = 0, hash_size = hash_item.size(); hash_pos < hash_size; ++hash_pos) {
            result = (result * 16777619) ^ hash_item[hash_pos];
        }

    }

    return result;

  }

  size_t hash<std::shared_ptr<construct>>::operator()(const std::shared_ptr<construct> & element) const {
    if(!element) return 0;
    else return element->hash();
  }

  bool equal_to<std::shared_ptr<construct>>::operator()(std::shared_ptr<construct> self, std::shared_ptr<construct> that) const {
    if(self == that) return true;
    if(!self || !that) return false;
    return *self == *that;
  }

}

/**
 * top_level_name_seek
 * @param nodes List of srcml nodes
 * @param start_pos The position of a node to search children for name tag
 *
 * This function is used by extract_name to supply the function get_name with the
 * position of the starting name tag.
 * 
 * loop O(n)
 * 
 */
void top_level_name_seek(const srcml_nodes & nodes, int & start_pos) {

    if(nodes.at(start_pos)->get_type() != srcml_node::srcml_node_type::START) return;

    const std::string & start_tag = nodes.at(start_pos)->get_name();
    int name_start_pos = start_pos + 1;

    while(nodes.at(name_start_pos)->get_type() != srcml_node::srcml_node_type::START || nodes.at(name_start_pos)->get_name() != "name") {

      if(nodes.at(name_start_pos)->get_type() == srcml_node::srcml_node_type::END && nodes.at(name_start_pos)->get_name() == start_tag)
        return;

      if(nodes.at(name_start_pos)->get_type() == srcml_node::srcml_node_type::START) {
        skip_tag(nodes, name_start_pos);
      }
      else {
        ++name_start_pos;
      }

    }

    start_pos = name_start_pos;

}

/**
 * skip_tag
 * @param nodes List of srcml nodes
 * @param start_pos The position of starting tag to be skipped
 *
 * This function can be used to skip a matching set of XML tags at the given
 * start position. If the position does not correspond with a starting tag 
 * an invalid argument exception is thrown.
 * 
 * loop 0(n)
 * 
 */
void skip_tag(const srcml_nodes & nodes, int & start_pos) {

  if(!nodes.at(start_pos)->is_open_tag()) throw std::invalid_argument("skip_tag: start is not open tag");

  const std::string & start_tag = nodes.at(start_pos)->get_name();
  int open_type_count = 1;
  ++start_pos;

  while(open_type_count) {

    if(nodes.at(start_pos)->get_name() == start_tag) {

      if(nodes.at(start_pos)->get_type() == srcml_node::srcml_node_type::START) {
        ++open_type_count;
      }
      else if(nodes.at(start_pos)->get_type() == srcml_node::srcml_node_type::END) {
        --open_type_count;
      }

    }

    ++start_pos;

  }

}

/**
 * get_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * @pre parameter name_start_pos must be the location of a node containing an open name tag
 *
 * This function can be used to get the name contents between an open XML name tag 
 * and its corresponding closing name tag 
 * 
 * loop 0(n)
 *
 * @returns string containing what was found in the name tag set 
 *          or empty string when not found
 *
 */

std::string get_name(const srcml_nodes & nodes, int name_start_pos) {

  int open_name_count = 1;
  int name_pos = name_start_pos + 1;
  std::string name = "";

  while(open_name_count) {

    if(nodes.at(name_pos)->get_type() == srcml_node::srcml_node_type::START && nodes.at(name_pos)->get_name() == "argument_list") return name;

    if(nodes.at(name_pos)->get_name() == "name") {

      if(nodes.at(name_pos)->get_type() == srcml_node::srcml_node_type::START) {
        ++open_name_count;
      }
      else if(nodes.at(name_pos)->get_type() == srcml_node::srcml_node_type::END) {
        --open_name_count;
      }

    } else if(nodes.at(name_pos)->is_text() && !nodes.at(name_pos)->is_whitespace()) {

      name += nodes.at(name_pos)->content ? *nodes.at(name_pos)->content : "";

    }

    ++name_pos;

  }

  return name;

}


/**
 * extract_name
 * @param nodes list of srcml nodes
 * @param start_pos position of starting tag to begin at
 *
 * This function can be used to extract a name from a set of name tags
 *
 * loop 0(n)
 * 
 * @returns  string containing what was found in the top-level most name tag set
 *           or empty string when nothing can be found
 *
 */

std::string extract_name(const srcml_nodes & nodes, int start_pos) {

  int name_start_pos = start_pos;
  top_level_name_seek(nodes, name_start_pos);

  if(name_start_pos == start_pos) return "";

  return get_name(nodes, name_start_pos);  

}