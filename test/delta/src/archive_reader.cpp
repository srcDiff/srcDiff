// SPDX-License-Identifier: GPL-3.0-only
/**
 * @file archive_reader.cpp
 *
 * @copyright Copyright (C) 2024-2024 SDML (www.srcDiff.org)
 *
 * This file is part of the srcDiff Infrastructure.
 */

#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <srcml.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <unordered_map>

const char * INSERT_TAG = "insert";
const char * DELETE_TAG = "delete";
const char * COMMON_TAG = "common";
const char * WS_TAG = "ws";

const char * DIFF_NS_PREFIX = "diff";
const char * DIFF_NS_URL = "http://www.srcML.org/srcDiff";


/**
 * Low-level utility function.
 * 
 * Checks to see if a node is an element node with the tag name
 * <diff:[tag_name]>.
*/
bool check_diff_tag(xmlNodePtr node, const char * tag_name) {
    return (
        node->type == XML_ELEMENT_NODE &&
        xmlStrcmp(node->ns->prefix, BAD_CAST DIFF_NS_PREFIX) == 0 &&
        xmlStrcmp(node->name, BAD_CAST tag_name) == 0
    );
}


/**
 * Low-level utility function.
 * 
 * This is initially passed the children of an element that is being removed;
 * that is, the children of a <diff:insert> element if we're getting the
 * original unit, or the children of a <diff:delete> element if we want the
 * modified unit. It gathers the descendants of the element being deleted that
 * should replace it: the nearest <diff:common> elements, <diff:insert>s if
 * we're removing a <diff:delete>, or the <diff:delete>s if we're removing a
 * <diff:insert>.
*/
void get_surviving_descendants(
    xmlNodePtr descendants, const char * allowed_tag, std::vector<xmlNodePtr>& survivors
) {
    for (xmlNodePtr cur = descendants; cur != NULL; cur = cur->next) {
        if (check_diff_tag(cur, allowed_tag) || check_diff_tag(cur, COMMON_TAG)) {
                for (xmlNodePtr child = cur->children; child != NULL; child = child->next) {
                    survivors.push_back(child);
                }
        } else if (cur->children) {
            get_surviving_descendants(cur->children, allowed_tag, survivors);
        }
    }
}

/**
 * Revision extraction function.
 *
 * This converts a srcDiff unit into just the srcML for either the original
 * version of the code (revision == 0) or the modified code (revision == 1).
*/
void get_srcml_for_revision(xmlNodePtr node, int revision) {
    const char * allowed_tag = (revision == 0) ? DELETE_TAG : INSERT_TAG;
    const char * disallowed_tag = (revision == 1) ? DELETE_TAG : INSERT_TAG;
    xmlNodePtr next;
    for (xmlNodePtr cur = node; cur != NULL; cur = next) {
        if (check_diff_tag(cur, disallowed_tag)) {
            // if we've found an element that should be deleted, we still need
            // to check if it has children that should survive, and insert them
            // in its place if so.
            std::vector<xmlNodePtr> survivors;
            get_surviving_descendants(cur->children, allowed_tag, survivors);
            xmlNodePtr first_new_node = NULL;
            // iteratively add the replacing elements as previous siblings
            // (repeatedly adding next siblings would reverse their order)
            for (xmlNodePtr survivor : survivors) {
                xmlNodePtr new_node = xmlAddPrevSibling(cur, survivor);
                if (!first_new_node) {
                    first_new_node = new_node;
                }
            }
            if (first_new_node) {
                // the newly added siblings themselves need to be checked for
                // descendants that should be deleted, so set the next node to
                // be explored to be the first added node if any were added
                next = first_new_node;
            } else {
                next = cur->next;
            }
            xmlUnlinkNode(cur);
            xmlFreeNode(cur);
        } else if (check_diff_tag(cur, WS_TAG) ||
                    check_diff_tag(cur, COMMON_TAG) ||
                    check_diff_tag(cur, allowed_tag)) {
            // to get srcML, we need to replace all diff-namespace nodes with
            // their children, so this cleans up nodes whose children are all
            // allowed
            xmlNodePtr first_new_node = NULL;
            xmlNodePtr nextChild;
            for (xmlNodePtr child = cur->children; child != NULL; child = nextChild) {
                nextChild = child->next;
                xmlNodePtr new_node = xmlAddPrevSibling(cur, child);
                if (!first_new_node) {
                    first_new_node = new_node;
                }
            }
            if (first_new_node) {
                // we still need to check the newly-inserted nodes if they exist
                next = first_new_node;
            } else {
                next = cur->next;
            }
            xmlUnlinkNode(cur);
            xmlFreeNode(cur);
        } else {
            // sequentially and recursively explore normal, non-diff-related
            // nodes too
            next = cur->next;
            if (cur->children) {
                get_srcml_for_revision(cur->children, revision);
            }
        }
    }
}

/**
 * Recursively gathers the text from all the descendants of a node.
*/
std::string get_all_node_text(xmlNodePtr node) {
    std::string result = "";
    for (xmlNodePtr cur = node; cur != NULL; cur = cur->next) {
        if (cur->content) {
            result += (char *)cur->content;
        }
        if (cur->children) {
            result += get_all_node_text(cur->children);
        }
    }
    return result;
}

/**
 * Copies all the XML attributes of a node into an unordered_map. Used for the
 * --info flag.
*/
std::unordered_map<std::string, std::string> xml_attributes_to_map(xmlNodePtr node) {
    std::unordered_map<std::string, std::string> result;

    for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {

        std::string key((const char *)attr->name);
        xmlChar * xmlValue = xmlNodeListGetString(node->doc, attr->children, 1);
        std::string value((const char *)xmlValue);
        xmlFree(xmlValue);

        result[key] = value;
    }

    return result;
}

/**
 * Generic function to output the contents of an unordered_map as JSON. Used for
 * the --info flag.
*/
template<typename K, typename V>
std::string unordered_map_to_json(const std::unordered_map<K, V>& map) {
    std::stringstream ss;
    ss << "{\n";
    bool first = true;
    for (const auto& pair : map) {
        if (!first) {
            ss << ",\n";
        }
        first = false;
        ss << "    \"" << pair.first << "\": ";

        // Check if the value is a string, if so, add quotes
        if constexpr(std::is_same_v<V, std::string>) {
            ss << "\"" << pair.second << "\"";
        } else {
            ss << pair.second;
        }
    }
    ss << "\n}";
    return ss.str();
}

int main(int argc, char* argv[]) {

    CLI::App cli(
        "Extracts units and revisions from srcDiff archives.",
        "archive_reader"
    );

    xmlIndentTreeOutput = 0;

    // variables to store CLI option/flag inputs:
    int unit = -1;
    int revision = -1;
    bool get_info = false;
    bool output_src = false;
    std::string archive_file;

    auto info_flag = cli.add_flag(
        "--info",
        get_info,
        "Output the attributes and size of a unit or archive in JSON format."
    );

    auto unit_option = cli.add_option<int>(
        "--unit",
        unit,
        "Specify a unit within the archive for which to output data."
    )->check(CLI::PositiveNumber);

    auto revision_option = cli.add_option<int>(
        "--revision",
        revision,
        "Specify a revision (0 for original, 1 for modified) to output srcML or source code for."
    )->check(CLI::Range(0, 1))->needs(unit_option)->excludes(info_flag);

    cli.add_flag(
        "--output-src",
        output_src,
        "Output the original source code of a revision within a unit, instead of the srcML."
    )->needs(unit_option)->needs(revision_option)->excludes(info_flag);

    cli.add_option<std::string>(
        "file",
        archive_file,
        "The path to the srcML archive to read."
    )->required();

    CLI11_PARSE(cli, argc, argv);

    if (unit_option->empty() && !info_flag) {
        std::cerr << "No unit specified, but info flag not preset; no output available." << std::endl;
        exit(1);
    }

    srcml_archive * in_archive = srcml_archive_create();
    if (srcml_archive_read_open_filename(in_archive, archive_file.c_str()) != SRCML_STATUS_OK) {
        std::cerr << "Could not read file: " << archive_file << std::endl;
        exit(1);
    }

    int units_found = 0;
    srcml_unit* read_unit = nullptr;
    while ((read_unit = srcml_archive_read_unit(in_archive))) {
        ++units_found;

        // if a unit number was specified on the command line, and we've found
        // the unit corresponding to that number, process it according to the
        // revision or info options, or just output it if neither of those were
        // specified

        if (!unit_option->empty() && units_found == unit) {
            // we have to write the unit out into a new archive to make the
            // "revision" attribute appear on it, to match the output that we'll
            // get from srcDiff. in general, this will standardize the output
            srcml_archive * out_archive = srcml_archive_create();
            srcml_archive_enable_solitary_unit(out_archive);
            srcml_archive_register_namespace(out_archive, DIFF_NS_PREFIX, DIFF_NS_URL);
            char * archive_out_chars;
            size_t archive_out_size;
            srcml_archive_write_open_memory(out_archive, &archive_out_chars, &archive_out_size);
            if (srcml_archive_write_unit(out_archive, read_unit) != SRCML_STATUS_OK) {
                std::cerr << "Could not process unit " << unit << " from file "
                          << archive_file << " with libsrcml!" << std::endl;
                exit(1);
            }
            srcml_archive_close(out_archive);
            srcml_archive_free(out_archive);

            if (!revision_option->empty() || !info_flag->empty()) {

                // if we need to extract a specific revision or get the unit's
                // attributes, parse it with libxml2

                xmlDocPtr doc = xmlReadMemory(
                    archive_out_chars, archive_out_size, archive_file.c_str(), "UTF-8", 0
                );
                xmlNodePtr root = xmlDocGetRootElement(doc);

                if (!revision_option->empty()) {
                    // get the srcml corresponding to the specific revision,
                    // then either dump the text or the full srcML based on the
                    // output_src flag
                    get_srcml_for_revision(root->children, revision);
                    if (output_src) {
                        std::cout << get_all_node_text(root->children);
                    } else {
                        xmlBufferPtr buffer = xmlBufferCreate();
                        xmlOutputBufferPtr output = xmlOutputBufferCreateBuffer(buffer, NULL);
                        xmlNodeDumpOutput(output, doc, root, 0, 0, "UTF-8");
                        xmlOutputBufferFlush(output);
                        std::cout << (const char *) xmlBufferContent(buffer);
                        xmlOutputBufferClose(output);
                        xmlBufferFree(buffer);
                    }
                } else if (get_info) {
                    std::cout << unordered_map_to_json(xml_attributes_to_map(root));
                }
                
                xmlFreeDoc(doc);
                xmlCleanupParser();
            
            } else {
                // if neither the revision nor the info option were specified,
                // just output the unit
                std::cout << archive_out_chars;
            }
            srcml_unit_free(read_unit);
            break;
        }
        srcml_unit_free(read_unit);
    }
    srcml_archive_close(in_archive);
    srcml_archive_free(in_archive);

    if (!unit_option->empty() && unit > units_found) {
        std::cerr << "Unit number " << unit << " is out of range; the archive only has "
                  << units_found << "units." << std::endl;
        exit(1);
    }

    if (get_info && unit_option->empty()) {

        // if we're getting info on the whole archive (due to a unit number not
        // being specified), parse the whole thing to get the attributes from
        // its root tag. (we still need to iterate over the units, above, just
        // to find out how many there are.)

        xmlDocPtr doc = xmlReadFile(archive_file.c_str(), NULL, 0);
        if (doc == NULL) {
            std::cerr << "Failed to parse " << archive_file << " as XML." << std::endl;
            exit(1);
        }

        xmlNodePtr root = xmlDocGetRootElement(doc);

        auto attrs = xml_attributes_to_map(root);
        attrs["units"] = std::to_string(units_found);
        std::cout << unordered_map_to_json(attrs);

        xmlFreeDoc(doc);
        xmlCleanupParser();
    }

    return 0;
}
