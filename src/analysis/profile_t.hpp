#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <counts_t.hpp>
#include <versioned_string.hpp>

#include <vector>
#include <map>
#include <iostream>
#include <memory>

struct profile_t {

    private:

    public:

        typedef std::vector<std::shared_ptr<profile_t>> profile_list_t;

        size_t id;
        std::string type_name;
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

        profile_t(std::string type_name = "") : id(0), type_name(type_name), is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),  has_assignment(false),
            modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), assignment_count(0), total_count(0) {}

        void set_id(size_t id_count) {

            id = id_count;

        }

        virtual void set_name(versioned_string name) {

            set_name(name, boost::optional<std::string>());

        }

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {}

        virtual std::ostream & print(std::ostream & out) const {

            return out << *this;

        }

        friend std::ostream & operator<<(std::ostream & out, const profile_t & profile) {

            out << profile.type_name << ":"; 
            out << " Whitespace: " << profile.whitespace_count;
            out << "\tComment: " << profile.comment_count;
            out << "\tSyntax: " << profile.syntax_count;
            out << "\tTotal: " << profile.total_count;
            out << '\n';

            return out;

        }

};

#endif