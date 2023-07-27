/**
 * @file construct_hash.hpp
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

#ifndef INCLUDED_CONSTUCT_HASH_HPP
#define INCLUDED_CONSTUCT_HASH_HPP

#include <construct.hpp>

#include <string>
#include <memory>

namespace std {

template<>
struct hash<construct> {

  size_t operator()(const construct & element) const {

    if(element.hash()) {
        return *element.hash();
    }

    size_t result = 2166136261;
    for(std::size_t pos = 0, size = element.size(); pos < size; ++pos) {

        const shared_ptr<srcml_node> & term = element.term(pos);
        const string & hash_item = term->is_text() ? *term->content : term->name;
        for(size_t hash_pos = 0, hash_size = hash_item.size(); hash_pos < hash_size; ++hash_pos) {
            result = (result * 16777619) ^ hash_item[hash_pos];
        }

    }

    ((construct &)element).hash(result);
    return result;

  }

};

}


#endif