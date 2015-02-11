#ifndef INCLUDED_SRCDIFF_SUMMARY_HANDLER_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HANDLER_HPP

#include <srcdiff_type.hpp>
#include <profile_t.hpp>
#include <counts_t.hpp>
#include <versioned_string.hpp>

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

    struct srcdiff {

        srcdiff_type operation;
        bool is_change;
        bool is_move;
        int level;

        srcdiff(srcdiff_type operation, bool is_change, bool is_move) : operation(operation), is_change(is_change), is_move(is_move), level(0) {}

    };

private:

protected:

    size_t id_count;
    profile_t::profile_list_t & profile_list;

    std::vector<srcdiff> srcdiff_stack;
    std::vector<profile_t> profile_stack;

    std::vector<std::pair<size_t, size_t>> counting_profile_pos;

    std::map<std::string, counts_t> inserted;
    std::map<std::string, counts_t> deleted;
    std::map<std::string, counts_t> modified;

    counts_t insert_count;
    counts_t delete_count;
    counts_t change_count;
    counts_t total;

    std::string text;

    size_t name_count;
    versioned_string collected_name;

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

    bool is_summary(const std::string & name);

    void update_diff_map(std::map<std::string, counts_t> & map, const std::string & name, bool is_whitespace);
    void update_diff(const std::string & name, size_t profile_pos, bool is_whitespace);
    void count_diff(bool is_whitespace);

    void update_modified_map(std::map<std::string, counts_t> & map, const std::string & name);
    void update_modified_map_summary(std::map<std::string, counts_t> & map, const std::string & name);
    void update_modified(const std::string & name, size_t profile_pos);
    void count_modified();

    void process_characters();

public:

    srcdiff_summary_handler(profile_t::profile_list_t & profile_list);

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
