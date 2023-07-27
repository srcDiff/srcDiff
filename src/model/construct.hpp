#ifndef INCLUDED_CONSTRUCT_HPP
#define INCLUDED_CONSTRUCT_HPP

#include <srcdiff_vector.hpp>

#include <srcdiff_compare.hpp>
#include <srcml_nodes.hpp>

#include <boost/optional.hpp>

#include <iostream>
#include <memory>

class construct {

public:

    construct(const srcml_nodes & node_list) : node_list(node_list), terms(), hash_value() {}

    /** loop O(n) */
    construct(const construct & that) : node_list(that.node_list), terms(), hash_value(that.hash_value) {

        for(std::size_t pos = 0; pos < that.size(); ++pos) {
            terms.push_back(that.terms[pos]);
        }

    }

    /** loop O(n) */
    construct(const srcml_nodes & node_list, int & start) : node_list(node_list), hash_value() {

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
    }

    construct & operator=(construct that) {
        swap(that);
        return *this;
    }

    bool operator==(const construct & that) const {
        return srcdiff_compare::construct_compare((const void *)this, (const void *)&that, nullptr) == 0;

    }

    friend std::ostream & operator<<(std::ostream & out, const construct & that) {

        for(std::size_t pos = 0, size = that.size(); pos < size; ++pos) {
            out << *that.term(pos);
        }

        return out;

    }

    /// term access api ///
    std::size_t size() const {
        return terms.size();
    }

    bool empty() const {
        return terms.empty();
    }

    const std::shared_ptr<srcml_node> & term(std::size_t pos) const {
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
        return node_list[terms.back()];
    } 

    /// position info of element
    std::size_t start_position() const {
        return terms.front();
    }

    std::size_t end_position() const {
        return terms.back();
    }



    const srcml_nodes & nodes() const {
        return node_list;
    }

    boost::optional<std::size_t> hash() const {
        return hash_value;
    }

    void hash(std::size_t hash_value) {
        this->hash_value = hash_value;
    }

    const std::shared_ptr<srcml_node> & get_node(std::size_t pos) const {
        return term(pos);
    }

    const std::shared_ptr<srcml_node> & get_root() const {
        return get_node(0);
    }

    const std::string & get_node_name(std::size_t pos) const {
        return get_node(pos)->name;
    }
    const std::string & get_root_name() const {
        return get_node_name(0);
    }


protected:

    const srcml_nodes & node_list;

    std::vector<int> terms;
    boost::optional<std::size_t> hash_value;

    /// @todo remove, as should be part of node
    static bool is_white_space(const std::shared_ptr<srcml_node> & node) {

      // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
      return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && node->is_white_space();

    }

};

#include <construct_hash.hpp>

#endif
