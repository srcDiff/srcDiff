/**
 * @file construct_utils.hpp
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

#ifndef INCLUDED_CONSTRUCT_UTILS_HPP
#define INCLUDED_CONSTRUCT_UTILS_HPP

class construct;

#include <functional>
#include <string>
#include <memory>

#include <srcml_nodes.hpp>

namespace std {

  template<>
  struct hash<construct> {
    size_t operator()(const construct & element) const;
  };

  template<>
  struct hash<std::shared_ptr<construct>> {
    size_t operator()(const std::shared_ptr<construct> & element) const;
  };


  template<>
  struct equal_to<std::shared_ptr<construct>>{
    bool operator()(std::shared_ptr<construct> self, std::shared_ptr<construct> that) const;
  };

}

void top_level_name_seek(const srcml_nodes & nodes, int & start_pos);
void skip_tag(const srcml_nodes & nodes, int & start_pos);
std::string get_name(const srcml_nodes & nodes, int name_start_pos);
std::string extract_name(const srcml_nodes & nodes, int start_pos);

#endif