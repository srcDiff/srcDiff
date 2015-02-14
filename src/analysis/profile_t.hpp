#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <counts_t.hpp>
#include <versioned_string.hpp>

#include <vector>
#include <map>
#include <iostream>
#include <utility>
#include <memory>

class profile_t {

    private:

    public:

        typedef std::vector<std::shared_ptr<profile_t>> profile_list_t;

        size_t id;
        std::string type_name;
        srcdiff_type operation;

        bool is_modified;
        bool is_whitespace;
        bool is_comment;
        bool is_syntax;
        bool has_assignment;

        size_t modified_count;
        size_t whitespace_count;
        size_t comment_count;
        size_t syntax_count;
        size_t assignment_count;
        size_t total_count;

        std::map<std::string, counts_t> inserted;
        std::map<std::string, counts_t> deleted;
        std::map<std::string, counts_t> modified;

        std::vector<size_t> child_profiles;

        profile_t(std::string type_name, srcdiff_type operation) : id(0), type_name(type_name), operation(operation), is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),  has_assignment(false),
            modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), assignment_count(0), total_count(0) {}

        void set_id(size_t id_count) {

            id = id_count;

        }

        virtual void set_name(versioned_string name) {

            set_name(name, boost::optional<std::string>());

        }

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            child_profiles.push_back(profile->id);
            
        }

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {}

        virtual std::ostream & summary(std::ostream & out) const {

            out << type_name << ":"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            return out;

        }

};

#endif