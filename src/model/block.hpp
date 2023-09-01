/**
 * @file block.hpp
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

#ifndef INCLUDED_BLOCK_HPP
#define INCLUDED_BLOCK_HPP

#include <construct.hpp>

class block : public construct {

public:

    block(const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out)
        : construct(node_list, start, out), block_content_child() {}

    std::shared_ptr<const construct> block_content() const;
    virtual bool is_matchable_impl(const construct & modified) const;
private:
    mutable std::optional<std::shared_ptr<const construct>> block_content_child;
};

#endif
