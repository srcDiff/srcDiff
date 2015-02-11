#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <counts_t.hpp>
#include <versioned_string.hpp>

#include <vector>
#include <map>
#include <iostream>

struct profile_t {

    private:

    public:

        typedef std::vector<profile_t> profile_list_t;

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

        virtual void set_name(versioned_string name, const std::string & parent) {}

        std::ostream & pad(std::ostream & out, size_t num) const {

            for(int i = 0; i < num; ++i)
                out << '\t';

            return out;

        }

        std::ostream & print_profile(std::ostream & out, const profile_list_t & profiles) const {

            if(total_count == 0) return out;

            static int depth = 0;

            if(syntax_count == 0) {

                if(whitespace_count) pad(out, depth) << "Whitespace:\t" << whitespace_count;
                if(comment_count) pad(out, depth) << "Comment:\t" << comment_count;

                out << '\n';

                return out;

            }

            pad(out, depth) << *this;

            if(!inserted.empty()) {

                pad(out, depth + 1) << "--inserted--\n";

                for(std::pair<std::string, counts_t> entry : inserted)
                    pad(out, depth + 1) << entry.first << ":\t" << entry.second.total << '\n';

            }

             if(!deleted.empty()) {

                pad(out, depth + 1) << "--deleted--\n";

                for(std::pair<std::string, counts_t> entry : deleted)
                    pad(out, depth + 1) << entry.first << ":\t" << entry.second.total << '\n';

            }

             if(!modified.empty()) {

                pad(out, depth + 1) << "--modified--\n";

                for(std::pair<std::string, counts_t> entry : modified)
                    pad(out, depth + 1) << entry.first << ":\t" << entry.second << '\n';

            }

            out << "\n\n";
            for(size_t child_pos : child_profiles)
                profiles[child_pos].print_profile(out, profiles);

            return out;

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