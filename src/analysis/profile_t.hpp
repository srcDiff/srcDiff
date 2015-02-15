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

        enum impact_factor {

            NONE   = 0,
            LOW    = 1,
            MEDIUM = 2,
            HIGH   = 3

        };

        static size_t depth;

        size_t id;
        std::string type_name;
        srcdiff_type operation;

        size_t num_sub_profiles;

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

        profile_t(std::string type_name, srcdiff_type operation) : id(0), type_name(type_name), operation(operation), num_sub_profiles(0),
                                                                   is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),
                                                                   has_assignment(false), modified_count(0), whitespace_count(0), comment_count(0),
                                                                   syntax_count(0), assignment_count(0), total_count(0) {}

        void set_id(size_t id_count) {

            id = id_count;

        }

        void set_num_sub_profiles(size_t count) {

            num_sub_profiles = count;

        }

        virtual void set_name(versioned_string name) {

            set_name(name, boost::optional<std::string>());

        }

        virtual void set_name(versioned_string name, const boost::optional<std::string> & parent) {}

        virtual void add_child(const std::shared_ptr<profile_t> & profile) {

            child_profiles.push_back(profile->id);
            
        }

        virtual impact_factor calculate_impact_factor() const {

            size_t impact_factor_number = syntax_count / num_sub_profiles;

            if(impact_factor_number == 0)    return NONE;
            if(impact_factor_number <  0.1)  return LOW;
            if(impact_factor_number <  0.25) return MEDIUM;
            return HIGH;

        }

        virtual const char * const get_impact_factor() const {

            impact_factor factor = calculate_impact_factor();

            switch(factor) {

                case NONE:   return "None";
                case LOW:    return "Low";
                case MEDIUM: return "Medium";
                case HIGH:   return "High";

            }

        }

        virtual std::ostream & summary(std::ostream & out) const {

            pad(out) << type_name << ":"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            return out;

        }

        static std::ostream & pad(std::ostream & out) {

            for(size_t i = 0; i < depth; ++i)
                out << '\t';

            return out;

        }

};

#endif