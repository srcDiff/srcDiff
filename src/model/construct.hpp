/*
 * SPDX-License-Identifier: GPL-3.0-only

 * Copyright (C) 2011-2024  SDML (www.srcDiff.org)
 * This file is part of the srcDiff translator.
 */
#ifndef INCLUDED_CONSTRUCT_HPP
#define INCLUDED_CONSTRUCT_HPP

#include <srcdiff_output.hpp>
#include <srcml_nodes.hpp>
#include <construct_utils.hpp>

#include <nesting/rule_checker.hpp>

#include <srcdiff_measure.hpp>

#include <optional>
#include <memory>
#include <span>

class construct {

public:

    typedef std::vector<std::shared_ptr<const construct>> construct_list;
    typedef std::span<const std::shared_ptr<const construct>> construct_list_view;

    typedef std::function<bool (std::size_t & node_pos, const srcml_nodes & node_list, const void * context)> construct_filter;

    static bool is_non_white_space(std::size_t & node_pos, const srcml_nodes & node_list, const void * context);
    static bool is_match(std::size_t & node_pos, const srcml_nodes & nodes, const void * context);

    construct_list get_descendents(std::size_t start_pos, std::size_t end_pos,
                                   construct_filter filter = is_non_white_space,
                                   const void * context = nullptr) const;

    construct(const srcml_nodes & node_list, std::shared_ptr<srcdiff_output> out = std::shared_ptr<srcdiff_output>())
        : out(out), node_list(node_list), terms(), hash_value() {}
    construct(const construct* parent, std::size_t& start);

    construct(const construct & that) = delete;
    construct & operator=(construct that) = delete;

    virtual ~construct() {}

    bool operator==(const construct & that) const;
    bool operator!=(const construct & that) const;

    friend std::ostream & operator<<(std::ostream & out, const construct & that);

    const construct* parent() const;

    const std::shared_ptr<srcdiff_output> output() const;
    std::shared_ptr<srcdiff_output> output();

    void expand_children() const;

    const construct_list & children() const;
    construct_list & children();

    /// term access api ///
    std::size_t size() const;

    bool empty() const;

    const std::shared_ptr<srcML::node> & term(std::size_t pos) const;

    const std::vector<int> & get_terms() const;
    // @todo possibly remove this
    std::vector<int> & get_terms();

    const std::shared_ptr<srcML::node> & last_term() const;

    /// position info of element
    std::size_t start_position() const;
    std::size_t end_position() const;


    const srcml_nodes & nodes() const;
    const std::shared_ptr<srcML::node> & root_term() const;
    const std::string & term_name(std::size_t pos) const;
    const std::string & root_term_name() const;
    const std::shared_ptr<srcML::node> parent_term() const;
    const std::string & parent_term_name() const;


    std::size_t hash() const;
    virtual std::string to_string(bool skip_whitespace = false) const;

    std::shared_ptr<const construct> find_child(const std::string & name) const;
    construct_list find_descendents(std::shared_ptr<srcML::node> element) const;
    std::shared_ptr<const construct> find_best_descendent(const construct& match_construct) const;

    // Differencing Rules
    const std::shared_ptr<srcdiff_measure> & measure(const construct & modified) const;
    bool is_similar(const construct & modified) const;
    bool is_text_similar(const construct & modified) const;
    bool is_syntax_similar(const construct & modified) const;
    virtual bool is_syntax_similar_impl(const construct & modified) const;

    bool can_refine_difference(const construct & modified) const;

    bool is_matchable(const construct & modified) const;
    virtual bool is_matchable_impl(const construct & modified) const;
    bool is_match_similar(const construct & modified) const;

    virtual bool is_tag_convertable(const construct & modified) const;
    bool is_convertable(const construct & modified) const;
    virtual bool is_convertable_impl(const construct & modified) const;

    // can nest -> can it be placed inside
    bool is_nestable(const construct & modified) const;
    virtual bool can_nest(const construct & modified) const;

    // does it have a nest target that is better
    //virtual bool is_nest(const construct & modified) const;

protected:
    // Delegates rule object(s)
    std::shared_ptr<nesting::rule_checker> nest_checker;

protected:

    std::shared_ptr<srcdiff_output> out;

    const srcml_nodes & node_list;

    std::vector<int> terms;
    mutable std::optional<std::size_t> hash_value;

    const construct* parent_construct;
    mutable std::optional<construct_list> child_constructs;

    mutable std::unordered_map<int, std::shared_ptr<srcdiff_measure>> measures;

};


#endif
