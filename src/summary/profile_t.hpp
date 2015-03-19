#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <counts_t.hpp>
#include <versioned_string.hpp>
#include <srcdiff_type.hpp>
#include <summary_type.hpp>
#include <namespace_uri.hpp>
#include <srcdiff_macros.hpp>
#include <identifier_diff.hpp>

#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <memory>

class profile_t {

    private:

        static constexpr const char * const BULLET = "\u2022";

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
        versioned_string type_name;
        namespace_uri uri;
        srcdiff_type operation;
        bool is_replacement;
        size_t parent_id;

        bool is_modified;
        bool is_whitespace;
        bool is_comment;
        bool is_syntax;

        size_t modified_count;
        size_t whitespace_count;
        size_t comment_count;
        size_t syntax_count;
        size_t total_count;

        std::vector<size_t> child_profiles;
        std::vector<size_t> descendant_profiles;      

        std::map<versioned_string, size_t> identifiers;
        std::map<versioned_string, size_t> summary_identifiers;

        std::vector<std::shared_ptr<profile_t>> common_profiles;

    public:

        profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, size_t parent_id) :
                                                                   id(0), type_name(type_name), uri(uri), operation(operation), is_replacement(false), parent_id(parent_id),
                                                                   is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),
                                                                   modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), total_count(0) {}

        void set_id(size_t id_count) {

            id = id_count;

        }

        void set_operation(srcdiff_type operation) {

            this->operation = operation;
                        
        }

        virtual void set_name(versioned_string name) {

            set_name(name, boost::optional<versioned_string>());

        }

        virtual void add_identifier(const versioned_string & identifier) {

            if(identifier.has_original() && identifier.has_modified() && !identifier.is_common()) {

                identifier_diff ident_diff(identifier);

                ident_diff.compute_diff();

                std::map<versioned_string, size_t>::iterator itr = identifiers.find(ident_diff.get_diff());
                if(itr == identifiers.end()) {

                    identifiers.insert(itr, std::make_pair(ident_diff.get_diff(), 1));

                } else {

                    ++itr->second;

                    std::map<versioned_string, size_t>::iterator itersect_itr = summary_identifiers.find(ident_diff.get_diff());
                    if(itersect_itr == summary_identifiers.end()) summary_identifiers.insert(itersect_itr, *itr);                     
                    else                                               ++itersect_itr->second;

                }

            }

        }

        virtual void set_name(versioned_string name UNUSED, const boost::optional<versioned_string> & parent UNUSED) {}
        
        virtual void add_child(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

            child_profiles.insert(std::lower_bound(child_profiles.begin(), child_profiles.end(), profile->id), profile->id);

        }

        virtual void add_descendant(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

            descendant_profiles.insert(std::lower_bound(descendant_profiles.begin(), descendant_profiles.end(), profile->id), profile->id);
            
        }

        virtual void add_common(const std::shared_ptr<profile_t> & profile) {

            common_profiles.push_back(profile);

        }        

        virtual impact_factor calculate_impact_factor() const {

            double impact_factor_number = (double)syntax_count / descendant_profiles.size();

            if(impact_factor_number == 0)    return NONE;
            if(impact_factor_number <  0.1)  return LOW;
            if(impact_factor_number <  0.25) return MEDIUM;
            return HIGH;

        }

        virtual const char * get_impact_factor() const {

            impact_factor factor = calculate_impact_factor();

            switch(factor) {

                case NONE:   return "None";
                case LOW:    return "Low";
                case MEDIUM: return "Medium";
                case HIGH:   return "High";

            }

        }

        virtual std::ostream & summary(std::ostream & out, size_t summary_types UNUSED, const profile_list_t & profile_list UNUSED) const {

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

        static std::ostream & begin_line(std::ostream & out) {

            return pad(out) << BULLET << ' ';

        }

};

#endif
