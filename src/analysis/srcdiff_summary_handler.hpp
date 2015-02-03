#ifndef INCLUDED_SRCDIFF_SUMMARY_HANDLER_HPP
#define INCLUDED_SRCDIFF_SUMMARY_HANDLER_HPP

#include <srcSAXHandler.hpp>

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

    typedef std::map<std::string, bool> class_profiles_t;
    typedef std::map<std::string, bool> function_profiles_t;

private:

protected:

    enum DIFF_TYPE { COMMON, DELETE, INSERT };

    std::vector<DIFF_TYPE> diff_stack;

    class_profiles_t & class_profiles;
    function_profiles_t & function_profiles;

    std::vector<std::string> class_names;
    int in_class;

    std::string function_name;
    bool in_function;

    bool collect_name;
    std::string name;

public:

    srcdiff_summary_handler(class_profiles_t & class_profiles, function_profiles_t & function_profiles) : class_profiles(class_profiles), function_profiles(function_profiles),
        in_class(0), in_function(false), collect_name(false), name() {

            diff_stack.push_back(COMMON);

        }

    /**
     * startDocument
     *
     * SAX handler function for start of document.
     * Overide for desired behaviour.
     */
    virtual void startDocument() {}

    /**
     * endDocument
     *
     * SAX handler function for end of document.
     * Overide for desired behaviour.
     */
    virtual void endDocument() {}

    /**
     * startRoot
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of the root element.
     * Overide for desired behaviour.
     */
    virtual void startRoot(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {}

    /**
     * startUnit
     * @param localname the name of the element tag
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
                           const struct srcsax_attribute * attributes) {}

    /**
     * startElement
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param num_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param num_attributes the number of attributes on the tag
     * @param attributes list of attributes
     *
     * SAX handler function for start of an element.
     * Overide for desired behaviour.
     */
    virtual void startElement(const char * localname, const char * prefix, const char * URI,
                                int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                                const struct srcsax_attribute * attributes) {

        const std::string local_name(localname);

        const std::string & parent = srcml_element_stack.at(srcml_element_stack.size() - 2);

        if(URI == std::string("http://www.sdml.info/srcDiff")) {

            if(local_name == "delete") diff_stack.push_back(DELETE);
            else if(local_name == "insert") diff_stack.push_back(INSERT);
            else diff_stack.push_back(COMMON);

        } else if(local_name == "class") {

            ++in_class;

        } else if(local_name == "function") {

            in_function = true;

        } else if(local_name == "name" && (parent == "class" || parent == "function")) {

            collect_name = true;

        }

    }

    /**
     * endRoot
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of the root element.
     * Overide for desired behaviour.
     */
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {}

    /**
     * endUnit
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an unit.
     * Overide for desired behaviour.
     */
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {}

    /**
     * endElement
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an element.
     * Overide for desired behaviour.
     */
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {

        const std::string local_name(localname);

        const std::string & parent = srcml_element_stack.back();

        if(URI == std::string("http://www.sdml.info/srcDiff")) {

            if(local_name == "delete") diff_stack.push_back(DELETE);
            else if(local_name == "insert") diff_stack.push_back(INSERT);
            else diff_stack.push_back(COMMON);

        } if(local_name == "class") {

            --in_class;
            class_names.pop_back();

        } else if(local_name == "function") {

            in_function = false;

        } else if(local_name == "name" && (parent == "class" || parent == "function")) {

            collect_name = false;

            if(parent == "class") { 

                class_profiles[name] = true;
                class_names.push_back(name);


            } else if(parent == "function") {

                function_profiles[name] = true;
                function_name = name;

            }

            name = "";

        }

    }

    /**
     * charactersRoot
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling at the root level.
     * Overide for desired behaviour.
     */
    virtual void charactersRoot(const char * ch, int len) {}

    /**
     * charactersUnit
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling within a unit.
     * Overide for desired behaviour.
     */
    virtual void charactersUnit(const char * ch, int len) {

        if(diff_stack.back() != COMMON) {

            for(int i = 0; i < len; ++i)
                if(!isspace(ch[i])) {

                    if(in_class) {

                        for(const std::string & name : class_names)
                            class_profiles[name] = false;

                    }

                    if(in_function)
                        function_profiles[function_name] = false;

                }



        }

        if(collect_name) {

            name.append(ch, len);

        }

    }

};

#endif
