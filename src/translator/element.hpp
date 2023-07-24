#ifndef INCLUDED_ELEMENT_HPP
#define INCLUDED_ELEMENT_HPP

#include <srcdiff_vector.hpp>

#include <srcdiff_compare.hpp>
#include <srcml_nodes.hpp>

#include <boost/optional.hpp>

#include <iostream>
#include <memory>

class element_t : public srcdiff_vector<int> {

public:

    element_t(const srcml_nodes & node_list) : node_list(node_list), hash_value() {}

    /** loop O(n) */
    element_t(const element_t & set) : node_list(set.node_list), hash_value(set.hash_value) {

        for(size_type pos = 0; pos < set.size(); ++pos) {

            push_back(set.vec[pos]);

        }

    }

    /** loop O(n) */
    element_t(const srcml_nodes & node_list, int & start) : node_list(node_list), hash_value() {

      if((xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_TEXT && (xmlReaderTypes)node_list.at(start)->type != XML_READER_TYPE_ELEMENT) return;

      push_back(start);

      if(node_list.at(start)->is_empty || (xmlReaderTypes)node_list.at(start)->type == XML_READER_TYPE_TEXT) return;

      ++start;

      // track open tags because could have same type nested
      int is_open = 1;
      for(; is_open; ++start) {

        // skip whitespace
        if(is_white_space(node_list.at(start))) {
          continue;
        }

        push_back(start);

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

    element_t & operator=(element_t set) {

        std::swap(vec, set.vec);

        return *this;

    }

    bool operator==(const element_t & that) const {

        diff_nodes diff = { nodes(), that.nodes() };
        return srcdiff_compare::node_set_syntax_compare((const void *)this, (const void *)&that, &diff) == 0;

    }

    friend std::ostream & operator<<(std::ostream & out, const element_t & that) {

        for(std::size_t pos = 0, size = that.size(); pos < size; ++pos) {
            out << *that.nodes()[that.vec[pos]];
        }

        return out;

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
        return nodes().at(at(pos));
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
    boost::optional<std::size_t> hash_value;

    /// @todo remove, as should be part of node
    static bool is_white_space(const std::shared_ptr<srcml_node> & node) {

      // node is all whitespace (NOTE: in collection process whitespace is always a separate node)
      return (xmlReaderTypes)node->type == XML_READER_TYPE_TEXT && node->content && node->is_white_space();

    }

};

namespace std {

template<>
struct hash<element_t> {

  size_t operator()(const element_t & set) const {

    if(set.hash())
        return *set.hash();

    size_t result = 2166136261;
    for(std::size_t pos = 0, size = set.size(); pos < size; ++pos) {

        const shared_ptr<srcml_node> & node = set.nodes().at(set.at(pos));
        const string & hash_item = node->is_text() ? *node->content : node->name;
        for(size_t hash_pos = 0, hash_size = hash_item.size(); hash_pos < hash_size; ++hash_pos) {
            result = (result * 16777619) ^ hash_item[hash_pos];
        }

    }

    ((element_t &)set).hash(result);
    return result;

  }

};

}

#endif
