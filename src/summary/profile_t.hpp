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

#include <vector>
#include <map>
#include <set>
#include <utility>
#include <memory>

class profile_t {

    private:

    public:

        static std::shared_ptr<profile_t> unit_profile;

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


        std::shared_ptr<profile_t> parent;
        std::shared_ptr<profile_t> summary_parent;
        std::shared_ptr<profile_t> body;
        std::shared_ptr<profile_t> summary_profile;

        profile_list_t child_profiles;
        std::vector<versioned_string> identifiers;

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

        std::string raw;

        profile_list_t child_change_profiles;
        profile_list_t descendant_change_profiles;      

        std::map<identifier_utilities, size_t> all_identifiers;
        std::map<identifier_utilities, size_t> summary_identifiers;

        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> common_identifiers;
        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> deleted_identifiers;
        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> inserted_identifiers;
        std::map<std::string, std::vector<std::shared_ptr<profile_t>>> modified_identifiers;

    public:

        profile_t(std::string type_name = "", namespace_uri uri = SRC, srcdiff_type operation = SRCDIFF_COMMON);

        profile_t(std::string type_name, namespace_uri uri, srcdiff_type operation, const std::shared_ptr<profile_t> & summary_parent);

        void set_id(size_t id_count);
        void set_operation(srcdiff_type operation);
        void add_child(const std::shared_ptr<profile_t> & profile);

        virtual void set_name(versioned_string name);
        virtual void set_name(versioned_string name UNUSED, const boost::optional<versioned_string> & parent UNUSED);
        virtual void add_identifier(const versioned_string & identifier, const versioned_string & parent);        
        virtual void add_child_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED);
        virtual void add_descendant_change(const std::shared_ptr<profile_t> & profile, const versioned_string & parent UNUSED);

        virtual impact_factor calculate_impact_factor() const;
        virtual const char * get_impact_factor() const;
        virtual summary_output_stream & summary(summary_output_stream & out, size_t summary_types UNUSED) const;

        bool operator<(const profile_t & profile) const;
        friend bool operator<(const std::shared_ptr<profile_t> & profile_one, const std::shared_ptr<profile_t> & profile_two);
        bool operator==(const profile_t & profile) const;
        friend bool operator==(const std::shared_ptr<profile_t> & profile_one, const std::shared_ptr<profile_t> & profile_two);
        bool operator==(int profile_id) const;
        friend bool operator==(const std::shared_ptr<profile_t> & profile_one, size_t profile_id);

};

#endif
