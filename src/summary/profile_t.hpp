#ifndef INCLUDED_PROFILE_T_HPP
#define INCLUDED_PROFILE_T_HPP

#include <counts_t.hpp>
#include <versioned_string.hpp>
#include <srcdiff_type.hpp>
#include <summary_type.hpp>
#include <namespace_uri.hpp>
#include <srcdiff_macros.hpp>
#include <identifier_utilities.hpp>
#include <summary_output_stream.hpp>
#include <type_query.hpp>

#include <vector>
#include <map>
#include <set>
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

        size_t id;
        versioned_string type_name;
        namespace_uri uri;
        srcdiff_type operation;
        bool is_replacement;
        size_t move_id;
        std::shared_ptr<profile_t> move_parent;

        std::shared_ptr<profile_t> parent;

        size_t statement_count_original;
        size_t statement_count_modified;
        size_t statement_count;
        size_t statement_churn;
        size_t common_statements;

        bool is_modified;
        bool is_whitespace;
        bool is_comment;
        bool is_syntax;

        size_t modified_count;
        size_t whitespace_count;
        size_t comment_count;
        size_t syntax_count;
        size_t total_count;

        bool left_hand_side;
        bool right_hand_side;
        profile_list_t common_profiles;

        profile_list_t child_profiles;
        profile_list_t descendant_profiles;      

        std::map<identifier_utilities, size_t> identifiers;
        std::map<identifier_utilities, size_t> summary_identifiers;

        std::string raw;

        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> common_identifiers;
        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> changed_identifiers;

    public:

        profile_t(std::string type_name = "", namespace_uri uri = SRC, srcdiff_type operation = SRCDIFF_COMMON) :
                                                                   id(0), type_name(type_name), uri(uri), operation(operation), is_replacement(false), move_id(0), move_parent(),
                                                                   parent(), statement_count_original(0), statement_count_modified(0), statement_count(0), statement_churn(0), common_statements(0),
                                                                   is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),
                                                                   modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), total_count(0),
                                                                   left_hand_side(false), right_hand_side(false), raw() {}

        profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & parent) :
                                                                   id(0), type_name(type_name), uri(uri), operation(operation), is_replacement(false), move_id(0), move_parent(),
                                                                   parent(parent), statement_count_original(0), statement_count_modified(0), statement_count(0), statement_churn(0), common_statements(0),
                                                                   is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),
                                                                   modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), total_count(0),
                                                                   left_hand_side(false), right_hand_side(false), raw() {}

        void set_id(size_t id_count) {

            id = id_count;

        }

        void set_operation(srcdiff_type operation) {

            this->operation = operation;
                        
        }

        virtual void set_name(versioned_string name) {

            set_name(name, boost::optional<versioned_string>());

        }

        virtual void add_identifier(const versioned_string & identifier, const versioned_string & parent) {

            if(identifier.has_original() && identifier.has_modified() && !identifier.is_common()) {

                identifier_utilities ident_diff(identifier);

                ident_diff.trim(is_call(parent));

                std::map<identifier_utilities, size_t>::iterator itr = identifiers.find(ident_diff);
                if(itr == identifiers.end()) {

                    identifiers.insert(itr, std::make_pair(ident_diff, 1));

                } else {

                    ++itr->second;

                    std::map<identifier_utilities, size_t>::iterator itersect_itr = summary_identifiers.find(ident_diff);
                    if(itersect_itr == summary_identifiers.end()) summary_identifiers.insert(itersect_itr, *itr);                     
                    else                                               ++itersect_itr->second;

                }

            }

        }

        virtual void set_name(versioned_string name UNUSED, const boost::optional<versioned_string> & parent UNUSED) {}
        
        virtual void add_child(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

            child_profiles.insert(std::lower_bound(child_profiles.begin(), child_profiles.end(), profile), profile);

        }

        virtual void add_descendant(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

            descendant_profiles.insert(std::lower_bound(descendant_profiles.begin(), descendant_profiles.end(), profile), profile);
            
        }

        virtual void add_common(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED) {

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

        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types UNUSED) const {

            out.pad() << type_name << ":"; 
            out << " Whitespace: " << whitespace_count;
            out << "\tComment: " << comment_count;
            out << "\tSyntax: " << syntax_count;
            out << "\tTotal: " << total_count;
            out << '\n';

            return out;

        }

        bool operator<(const profile_t & profile) const {

            return id < profile.id;

        }

        friend bool operator<(const std::shared_ptr<profile_t> & profile_one, const std::shared_ptr<profile_t> & profile_two) {

            return profile_one->id < profile_two->id;

        }

        bool operator==(const profile_t & profile) const {

            return id == profile.id;

        }

        friend bool operator==(const std::shared_ptr<profile_t> & profile_one, const std::shared_ptr<profile_t> & profile_two) {

            return profile_one->id == profile_two->id;

        }

        bool operator==(int profile_id) const {

            return id == profile_id;

        }

        friend bool operator==(const std::shared_ptr<profile_t> & profile_one, size_t profile_id) {

            return profile_one->id == profile_id;

        }

};

#endif
