/**
 * @file construct.hpp
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

#ifndef INCLUDED_CONSTRUCT_HPP
#define INCLUDED_CONSTRUCT_HPP

#include <srcdiff_output.hpp>
#include <srcml_nodes.hpp>
#include <construct_hash.hpp>

#include <srcdiff_measure.hpp>

#include <optional>
#include <memory>

class construct {

public:

    typedef std::vector<construct> construct_list;
    typedef std::function<bool (int & node_pos, const srcml_nodes & node_list, const void * context)> construct_filter;

    static bool is_non_white_space(int & node_pos, const srcml_nodes & node_list, const void * context);

    /// @todo make member.  Requires modifiying a lot of methods in other classes.
    // name does not quite match because not a member yet.
    static construct_list get_descendent_constructs(const srcml_nodes & node_list, 
                                                    std::size_t start_pos, std::size_t end_pos,
                                                    construct_filter filter = is_non_white_space,
                                                    const void * context = nullptr,
                                                    std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>());

    construct(const srcml_nodes & node_list, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>())
        : out(out), node_list(node_list), terms(), hash_value() {}

    construct(const construct & that);

    construct(const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>());

    void swap(construct & that);
    construct & operator=(construct that);

    bool operator==(const construct & that) const;
    bool operator!=(const construct & that) const;

    friend std::ostream & operator<<(std::ostream & out, const construct & that);

    void expand_children() const;

    const construct_list & children() const;
    construct_list & children();

    /// term access api ///
    std::size_t size() const;

    bool empty() const;

    const std::shared_ptr<srcml_node> & term(std::size_t pos) const;

    const std::vector<int> & get_terms() const;
    // @todo possibly remove this
    std::vector<int> & get_terms();

    const std::shared_ptr<srcml_node> & last_term() const;

    /// position info of element
    std::size_t start_position() const;
    std::size_t end_position() const;


    const srcml_nodes & nodes() const;
    const std::shared_ptr<srcml_node> & root_term() const;
    const std::string & term_name(std::size_t pos) const;
    const std::string & root_term_name() const;

    std::optional<std::size_t> hash() const;


    // Differencing Rules
    const std::shared_ptr<srcdiff_measure> & measure(const construct & modified) const;
    bool is_similar(const construct & modified) const;

protected:
    std::shared_ptr<srcdiff_output> out;

    const srcml_nodes & node_list;

    std::vector<int> terms;
    mutable std::optional<std::size_t> hash_value;

    mutable std::optional<construct_list> child_constructs;

    mutable std::map<int, std::shared_ptr<srcdiff_measure>> measures;

};


#endif
