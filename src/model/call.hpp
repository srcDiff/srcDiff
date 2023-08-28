/**
 * @file call.hpp
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

#ifndef INCLUDED_CALL_HPP
#define INCLUDED_CALL_HPP

#include <construct.hpp>

#include <vector>

class call : public construct {

public:

    call(const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out) : construct(node_list, start, out) {}
    virtual bool is_matchable_impl(const construct & modified) const;

private:
    void collect_names();
private:
    mutable std::vector<std::string> names;
};


#endif
