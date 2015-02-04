#ifndef INCLUDED_SRCDIFF_SUMMARY_HANDLER_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HANDLER_HPP

#include <srcSAXHandler.hpp>

#include <iostream>
#include <string>
#include <map>

#include <cstdlib>

/**
 * srcSAXHandler
 *
 * Base class that provides hooks for SAX processing.
 */
class srcdiff_summary_handler : public srcSAXHandler {

public:

    enum srcdiff_type {

        SRCDIFF_COMMON,
        SRCDIFF_INSERT,
        SRCDIFF_DELETE,

    };

    struct srcdiff {

        srcdiff_type operation;
        bool is_change;
        bool is_move;
        int level;

        srcdiff(srcdiff_type operation, bool is_change, bool is_move) : operation(operation), is_change(is_change), is_move(is_move), level(0) {}

    };


    struct counts {

        counts() : whitespace(0), comment(0), syntax(0), total(0) {}

        counts(int whitespace, int comment, int syntax, int total) : whitespace(whitespace), comment(comment), syntax(syntax), total(total) {}

        void inc_whitespace() {

            ++whitespace;

        }

         void inc_comment() {

            ++comment;

        }

        void inc_syntax() {

            ++syntax;

        }

        void inc_total() {

            ++total;

        }

        bool operator==(const counts & other_count) const {


            return whitespace == other_count.whitespace && comment == other_count.comment && syntax == other_count.syntax && total == other_count.total;

        }

        bool operator!=(const counts & other_count) const {


            return !(*this == other_count);

        }

        friend std::ostream & operator<<(std::ostream & out, const counts & count) {


            return out << "Whitespace: " << count.whitespace << " Comment: " << count.comment << " Syntax: " << count.syntax << " Total: " << count.total;

        }

        int whitespace;
        int comment;
        int syntax;
        int total;

    };

    struct profile_t {

        private:

            static size_t id_count;

        public:

            size_t id;
            std::string type_name;
            std::string name;
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

            std::map<std::string, counts> inserted;
            std::map<std::string, counts> deleted;
            std::map<std::string, counts> modified;

            std::vector<size_t> child_profiles;

            profile_t(std::string type_name = "") : id(0), type_name(type_name), name(), is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),  has_assignment(false),
                modified_count(0), whitespace_count(0), comment_count(0), syntax_count(0), assignment_count(0), total_count(0) {}

            void set_id() {

                id = ++id_count;

            }

            friend std::ostream & operator<<(std::ostream & out, const profile_t & profile) {


                return out << profile.type_name << " '" << profile.name 
                    << "': Whitespace: " << profile.whitespace_count << " Comment: " << profile.comment_count << " Syntax: " << profile.syntax_count << " Total: " << profile.total_count;

            }

    };

    typedef std::vector<profile_t> profile_list_t;

private:

protected:

    profile_list_t & profile_list;

    std::vector<srcdiff> srcdiff_stack;
    std::vector<profile_t> profile_stack;

    std::vector<size_t> counting_profile_pos;

    std::map<std::string, counts> inserted;
    std::map<std::string, counts> deleted;
    std::map<std::string, counts> modified;

    counts insert_count;
    counts delete_count;
    counts change_count;
    counts total;

    std::string text;

    size_t name_count;
    std::string collected_name;

private:

    bool is_simple_type(const std::string & name);
    bool is_funct_type(const std::string & name);
    bool is_class_type(const std::string & name);
    bool is_template(const std::string & name);
    bool is_condition_type(const std::string & name);
    bool is_catch(const std::string & name);
    bool is_call(const std::string & name);
    bool is_decl_stmt(const std::string & name);
    bool is_preprocessor_special(const std::string & name);
    bool is_expr(const std::string & name);
    bool is_count(const std::string & name);

    void update_diff_map(std::map<std::string, counts> & map, const std::string & name, bool is_whitespace);
    void update_diff(const std::string & name, size_t profile_pos, bool is_whitespace);
    void count_diff(bool is_whitespace);

    void update_modified(const std::string & name, size_t profile_pos);
    void count_modified();

    void process_characters();

public:

    srcdiff_summary_handler(profile_list_t & profile_list);

    /**
     * startDocument
     *
     * SAX handler function for start of document.
     * Overide for desired behaviour.
     */
    virtual void startDocument();

    /**
     * endDocument
     *
     * SAX handler function for end of document.
     * Overide for desired behaviour.
     */
    virtual void endDocument();

    /**
     * startRoot
     * @param localname the name of the profile tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of the root profile.
     * Overide for desired behaviour.
     */
    virtual void startRoot(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes);

    /**
     * startUnit
     * @param localname the name of the profile tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of an unit.
     * Overide for desired behaviour.
     */
    virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes);

    /**
     * startElement
     * @param localname the name of the profile tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of an profile.
     * Overide for desired behaviour.
     */
    virtual void startElement(const char * localname, const char * prefix, const char * URI,
                                int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                                const struct srcsax_attribute * attributes);

    /**
     * endRoot
     * @param localname the name of the profile tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of the root profile.
     * Overide for desired behaviour.
     */
    virtual void endRoot(const char * localname, const char * prefix, const char * URI);

    /**
     * endUnit
     * @param localname the name of the profile tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an unit.
     * Overide for desired behaviour.
     */
    virtual void endUnit(const char * localname, const char * prefix, const char * URI);

    /**
     * endElement
     * @param localname the name of the profile tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an profile.
     * Overide for desired behaviour.
     */
    virtual void endElement(const char * localname, const char * prefix, const char * URI);

    /**
     * charactersRoot
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling at the root level.
     * Overide for desired behaviour.
     */
    virtual void charactersRoot(const char * ch, int len);

    /**
     * charactersUnit
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling within a unit.
     * Overide for desired behaviour.
     */
    virtual void charactersUnit(const char * ch, int len);

};

#endif
