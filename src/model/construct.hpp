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
#include <srcdiff_compare.hpp>
#include <srcml_nodes.hpp>

#include <boost/optional.hpp>

#include <iostream>
#include <memory>

#include <construct_hash.hpp>

class construct {

public:

    typedef std::vector<construct> construct_list;
    typedef std::function<bool (int & node_pos, const srcml_nodes & node_list, const void * context)> construct_filter;

    /// @todo remove, as should be part of node
    static bool is_white_space(const std::shared_ptr<srcml_node> & node) {

      // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
      return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && node->is_white_space();

    }

    static bool is_non_white_space(int & node_pos, const srcml_nodes & node_list, const void * context) {

        const std::shared_ptr<srcml_node> & node = node_list[node_pos];

        // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
        return (xmlReaderTypes)node->type == XML_READER_TYPE_ELEMENT || ((xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && !node->is_white_space());

    }

    /// @todo make member.  Requires modifiying a lot of methods in other classes.
    // name does not quite match because not a member yet.
    static construct_list get_descendent_constructs(const srcml_nodes & node_list, std::size_t start_pos, std::size_t end_pos, construct_filter filter = is_non_white_space, const void * context = nullptr, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>()) {
        construct_list descendent_constructs;

        // runs on a subset of base array
        for(int pos = start_pos; pos < end_pos; ++pos) {

            // skip whitespace
            if(filter(pos, node_list, context)) {

                // text is separate node if not surrounded by a tag in range
                if((xmlReaderTypes)node_list.at(pos)->type == XML_READER_TYPE_TEXT || (xmlReaderTypes)node_list.at(pos)->type == XML_READER_TYPE_ELEMENT) {
                    descendent_constructs.emplace_back(node_list, pos, out);
                } else {
                    return descendent_constructs;
                }

            }

        }
        return descendent_constructs;
    }

    construct(const srcml_nodes & node_list, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>()) : out(out), node_list(node_list), terms(), hash_value() {}

    /** loop O(n) */
    construct(const construct & that) : out(that.out), node_list(that.node_list), terms(), hash_value(that.hash_value) {

        for(std::size_t pos = 0; pos < that.size(); ++pos) {
            terms.push_back(that.terms[pos]);
        }

    }

    /** loop O(n) */
    construct(const srcml_nodes & node_list, int & start, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>()) : out(out), node_list(node_list), hash_value() {

      if((xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_TEXT && (xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_ELEMENT) return;

      terms.push_back(start);

      if(node_list.at(start)->is_empty || (xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_TEXT) return;

      ++start;

      // track open tags because could have same type nested
      int is_open = 1;
      for(; is_open; ++start) {

        // skip whitespace
        if(is_white_space(node_list.at(start))) {
          continue;
        }

        terms.push_back(start);

        // opening tags
        if((xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_ELEMENT
           && !(node_list.at(start)->is_empty)) {
          ++is_open;
        }

        // closing tags
        else if((xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_END_ELEMENT) {
          --is_open;
        }

      }

      --start;
    }

    void swap(construct & that) {
        std::swap(terms, that.terms);
        std::swap(hash_value, that.hash_value);
        std::swap(child_constructs, that.child_constructs);
    }

    construct & operator=(construct that) {
        swap(that);
        return *this;
    }

    bool operator==(const construct & that) const {

        if(!(hash() == that.hash())) return false;

        for(std::size_t i = 0, j = 0; i < size() && j < that.size();) {

          // string consecutive non whitespace text nodes
          if(term(i)->is_text() && that.term(j)->is_text()) {

            std::string text1 = "";
            for(; i < size() && term(i)->is_text(); ++i) {
              text1 += term(i)->content ? *term(i)->content : "";
            }

            std::string text2 = "";
            for(; j < that.size() && that.term(j)->is_text(); ++j) {
              text2 += that.term(j)->content ? *that.term(j)->content : "";
            }

            if(text1 != text2) return false;

          } else {

            if(*term(i) != *that.term(j)) return false;

            ++i;
            ++j;

          }
        }

        return true;
    }

    friend std::ostream & operator<<(std::ostream & out, const construct & that) {

        for(std::size_t pos = 0, size = that.size(); pos < size; ++pos) {
            out << *that.term(pos);
        }

        return out;

    }

    void expand_children() const {
        child_constructs = get_descendent_constructs(node_list, start_position() + 1, end_position(), is_non_white_space, nullptr, out);
    }

    const construct_list & children() const {
        if(!child_constructs) expand_children();
        return *child_constructs;
    }

    construct_list & children() {
        if(!child_constructs) expand_children();
        return *child_constructs;
    }


    /// term access api ///
    std::size_t size() const {
        return terms.size();
    }

    bool empty() const {
        return terms.empty();
    }

    const std::shared_ptr<srcml_node> & term(std::size_t pos) const {
        assert(pos < terms.size());
        return node_list[terms[pos]];
    }

    const std::vector<int> & get_terms() const {
        return terms;
    }

    // @todo possibly remove this
    std::vector<int> & get_terms() {
        return terms;
    }

    const std::shared_ptr<srcml_node> & last_term() const {
        assert(!node_list.empty());
        return node_list[terms.back()];
    } 

    /// position info of element
    std::size_t start_position() const {
        assert(!terms.empty());
        return terms.front();
    }

    std::size_t end_position() const {
        assert(!terms.empty());
        return terms.back();
    }



    const srcml_nodes & nodes() const {
        return node_list;
    }

    boost::optional<std::size_t> hash() const {
        if(!hash_value) hash_value = std::hash<construct>()(*this);
        return hash_value;
    }

    const std::shared_ptr<srcml_node> & root_term() const {
        return term(0);
    }

    const std::string & term_name(std::size_t pos) const {
        return term(pos)->name;
    }
    const std::string & root_term_name() const {
        return term_name(0);
    }


protected:
    std::shared_ptr<srcdiff_output> out;

    const srcml_nodes & node_list;

    std::vector<int> terms;
    mutable boost::optional<std::size_t> hash_value;

    mutable boost::optional<construct_list> child_constructs;

};


#endif
