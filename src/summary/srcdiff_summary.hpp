#ifndef INCLUDED_SRCDIFF_SUMMARY_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HPP

#include <srcSAXHandler.hpp>

#include <srcdiff_type.hpp>
#include <summary_type.hpp>
#include <namespace_uri.hpp>
#include <profile_t.hpp>
#include <counts_t.hpp>
#include <versioned_string.hpp>

#include <cctype>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <tuple>

#include <cstdlib>

/**
 * srcSAXHandler
 *
 * Base class that provides hooks for SAX processing.
 */
class srcdiff_summary : public srcSAXHandler {

public:

    struct srcdiff {

        srcdiff_type operation;
        bool is_change;
        size_t move_id;
        int level;

        srcdiff(srcdiff_type operation, bool is_change, size_t move_id) : operation(operation), is_change(is_change), move_id(move_id), level(0) {}

    };

private:

protected:

	std::ostream * out;

    size_t summary_types;    

    size_t id_count;
    std::shared_ptr<profile_t> unit_profile;

    std::vector<srcdiff> srcdiff_stack;
    std::vector<std::shared_ptr<profile_t>> profile_stack;
    std::vector<namespace_uri> uri_stack;

    std::vector<std::tuple<size_t, size_t, size_t>> counting_profile_pos;

    size_t expr_stmt_pos;
    size_t function_pos;

    counts_t insert_count;
    counts_t delete_count;
    counts_t change_count;
    counts_t total;

    std::string text;

    size_t name_count;
    versioned_string collected_name;

    size_t condition_count;
    versioned_string collected_condition;

    versioned_string collect_lhs;

private:

    void process_characters();
    void update_anscestor_profile(const std::shared_ptr<profile_t> & profile);
    void update_common_profiles(const std::shared_ptr<profile_t> & profile);
public:

    srcdiff_summary(const std::string & output_filename, const boost::optional<std::string> & summary_type_str);
    ~srcdiff_summary();

    void summarize(const std::string & srcdiff, const std::string & xml_encoding);
    void summarize(const std::shared_ptr<profile_t> & profile);
    void reset();

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
