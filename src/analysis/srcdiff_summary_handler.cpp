#include <srcdiff_summary_handler.hpp>

#include <type_query.hpp>

#include <unit_profile_t.hpp>
#include <function_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>

#include <cstring>

bool is_count(const std::string & type_name) {

return is_function_type(type_name) || is_class_type(type_name) || is_simple_type(type_name)
    || is_condition_type(type_name) || is_catch(type_name) || is_decl_stmt(type_name)
    || is_call(type_name) || is_preprocessor_special(type_name) || is_expr(type_name)
    || is_template(type_name) || is_parameter(type_name) || is_lambda(type_name);

}

bool is_summary(const std::string & type_name) {

    return is_class_type(type_name) || is_function_type(type_name);

}

std::shared_ptr<profile_t> make_profile(const std::string & type_name, namespace_uri uri, srcdiff_type operation) {

    if(is_class_type(type_name))    return std::make_shared<class_profile_t>(type_name, uri, operation);
    if(is_function_type(type_name)) return std::make_shared<function_profile_t>(type_name, uri, operation);
    if(is_parameter(type_name))     return std::make_shared<parameter_profile_t>(type_name, uri, operation);
    if(is_decl_stmt(type_name))     return std::make_shared<decl_stmt_profile_t>(type_name, uri, operation);

    return std::make_shared<profile_t>(type_name, uri, operation);

}

void srcdiff_summary_handler::process_characters() {

    const char * ch = text.c_str();
    std::string::size_type len = text.size();

    if(srcdiff_stack.back().operation != SRCDIFF_COMMON && srcdiff_stack.back().level == 0) {

        const char * current_ch = ch;

        while(current_ch < (ch + len)) {

            bool is_whitespace = true;
            const char * save_ch = current_ch;
            for(; current_ch < (ch + len); ++current_ch) {

                if(!isspace((char)*current_ch)) {

                    break;

                }

            }

            if(current_ch == save_ch) {

                is_whitespace = false;

                for(; current_ch < (ch + len); ++current_ch) {

                    if(isspace((char)*current_ch)) {

                        break;

                    }

                }

            }

            if(profile_stack.back()->type_name == "comment") {

                profile_stack.at(profile_stack.size() - 2)->is_comment = true;
                ++profile_stack.at(profile_stack.size() - 2)->comment_count;
                ++profile_stack.at(profile_stack.size() - 2)->total_count;
                total.inc_comment();

                if(srcdiff_stack.back().is_change) {

                    change_count.inc_comment();
                    change_count.inc_total();
                    
                } else if(srcdiff_stack.back().operation == SRCDIFF_DELETE) {

                    delete_count.inc_comment();
                    delete_count.inc_total();

                } else {

                    insert_count.inc_comment();
                    insert_count.inc_total();

                }

            } else if(is_whitespace) {

                profile_stack.at(profile_stack.size() - 2)->is_whitespace = true;
                ++profile_stack.at(profile_stack.size() - 2)->whitespace_count;
                ++profile_stack.at(profile_stack.size() - 2)->total_count;
                total.inc_whitespace();

                if(srcdiff_stack.back().is_change) {

                    change_count.inc_whitespace();
                    change_count.inc_total();
                    
                } else if(srcdiff_stack.back().operation == SRCDIFF_DELETE) {

                    delete_count.inc_whitespace();
                    delete_count.inc_total();

                } else {

                    insert_count.inc_whitespace();
                    insert_count.inc_total();

                }

            } else {

                profile_stack.at(profile_stack.size() - 2)->is_syntax = true;
                ++profile_stack.at(profile_stack.size() - 2)->syntax_count;
                ++profile_stack.at(profile_stack.size() - 2)->total_count;
                total.inc_syntax();

                if(srcdiff_stack.back().is_change) {

                    change_count.inc_syntax();
                    change_count.inc_total();
                    
                } else if(srcdiff_stack.back().operation == SRCDIFF_DELETE) {

                    delete_count.inc_syntax();
                    delete_count.inc_total();

                } else {

                    insert_count.inc_syntax();
                    insert_count.inc_total();

                }

            }

            total.inc_total();

            profile_stack.push_back(make_profile("text", SRC, srcdiff_stack.back().operation));

            profile_stack.pop_back();

        }


    }

    text = "";

}

srcdiff_summary_handler::srcdiff_summary_handler(profile_t::profile_list_t & profile_list) 
    : id_count(0), profile_list(profile_list), srcdiff_stack(), profile_stack(), counting_profile_pos(),
      insert_count(), delete_count(), change_count(), total(), text(), name_count(0), collected_name() {}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::startDocument() {}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::endDocument() {}

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
void srcdiff_summary_handler::startRoot(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    if(text != "") process_characters();

}

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
void srcdiff_summary_handler::startUnit(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    uri_stack.push_back(SRC);
    srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, false, false));

    if(text != "") process_characters();

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += localname;

    profile_stack.push_back(std::make_shared<unit_profile_t>(full_name, SRC, SRCDIFF_COMMON));

    for(int i = 0; i < num_attributes; ++i)
        if(attributes[i].localname == std::string("filename")) {

            std::string file_name = attributes[i].value;

            std::string::size_type pos = file_name.find('|');

            if(pos == std::string::npos) profile_stack.back()->set_name(versioned_string(file_name));
            else profile_stack.back()->set_name(versioned_string(file_name.substr(0, pos), file_name.substr(pos + 1)));

            break;

        }

    counting_profile_pos.push_back(std::make_pair<size_t, size_t>(profile_stack.size() - 1, profile_stack.size() - 1));
    profile_stack.back()->set_id(++id_count);

}

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
void srcdiff_summary_handler::startElement(const char * localname, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes) {

    const std::string local_name(localname);

    uri_stack.push_back(URI == std::string("http://www.sdml.info/srcDiff") ? SRCDIFF : (URI == std::string("http://www.sdml.info/srcML/src") ? SRC : CPP));

    // detect if interchange
    size_t srcml_depth = uri_stack.size();
    bool is_interchange = srcml_depth > 4 && uri_stack.at(srcml_depth - 4) == SRCDIFF && srcml_element_stack.at(srcml_depth - 4) == "diff:delete"
                            && uri_stack.at(srcml_depth - 3) == SRC && uri_stack.at(srcml_depth - 2) == SRCDIFF && srcml_element_stack.at(srcml_depth - 2) == "diff:insert";

    if(text != "") process_characters();

    if(uri_stack.back() == SRCDIFF) {

        /* @todo check if move and put as in common */
        bool is_change = false;
        bool is_move = false;
        for(int i = 0; i < num_attributes; ++i) {

            if(attributes[i].localname == std::string("type")) {

                std::string value = attributes[i].value;
                if(value == "change") is_change = true;

            } else if(attributes[i].localname == std::string("move")) {

                is_move = true;

            }

        }

        if(local_name == "common" || is_move)
            srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, is_change, is_move));
        else if(local_name == "delete")
            srcdiff_stack.push_back(srcdiff(SRCDIFF_DELETE, is_change, is_move));
        else if(local_name == "insert")
            srcdiff_stack.push_back(srcdiff(SRCDIFF_INSERT, is_change, is_move));

        bool is_interchange_diff = (srcml_depth > 3 && uri_stack.at(srcml_depth - 3) == SRCDIFF && srcml_element_stack.at(srcml_depth - 3) == "diff:delete"
                            && uri_stack.at(srcml_depth - 2) == SRC && uri_stack.back() == SRCDIFF && local_name == "insert");

        if((srcdiff_stack.back().operation == SRCDIFF_DELETE || srcdiff_stack.back().operation == SRCDIFF_INSERT)
       && (srcdiff_stack.at(srcdiff_stack.size() - 2).operation == SRCDIFF_COMMON || is_interchange_diff))
            profile_stack.back()->is_modified = true;

    }

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += local_name;

    if(is_interchange) {

        profile_stack.at(counting_profile_pos.back().first)->set_operation(SRCDIFF_COMMON);
        profile_stack.at(counting_profile_pos.back().first)->type_name.set_modified(full_name);

        std::shared_ptr<profile_t> & parent = profile_stack.at(counting_profile_pos.back().first - 2);
        size_t syntax_dec = profile_stack.at(counting_profile_pos.back().first)->type_name.original() == "else"
                            || profile_stack.at(counting_profile_pos.back().first)->type_name.modified() == "else" ? 1 : 2;
        parent->syntax_count -= syntax_dec;
        parent->total_count -= syntax_dec;

    } else {

        profile_stack.push_back(make_profile(full_name, uri_stack.back(), srcdiff_stack.back().operation));

    }

    if(uri_stack.back() != SRCDIFF) {

        if(local_name == "name") {

            ++name_count;

        }

        if(!is_interchange) ++srcdiff_stack.back().level;

        if(!is_interchange && is_count(full_name)) {

            bool summarize = is_summary(local_name);
            counting_profile_pos.push_back(std::make_pair<size_t, size_t>(profile_stack.size() - 1, summarize ? profile_stack.size() - 1 : counting_profile_pos.back().second));
            profile_stack.back()->set_id(++id_count);

        }

        if(srcdiff_stack.back().operation != SRCDIFF_COMMON) {

            if(srcdiff_stack.back().level == 1) {

                if(local_name == "comment") {

                    total.inc_comment();
                    profile_stack.at(profile_stack.size() - 3)->is_comment = true;
                    ++profile_stack.at(profile_stack.size() - 3)->comment_count;
                    ++profile_stack.at(profile_stack.size() - 3)->total_count;

                    /** @todo refactor into method or something for counting global insert/delete/change */
                    if(srcdiff_stack.back().is_change) {

                        change_count.inc_comment();
                        change_count.inc_total();
                        
                    } else if(srcdiff_stack.back().operation == SRCDIFF_DELETE) {

                        delete_count.inc_comment();
                        delete_count.inc_total();

                    } else {

                        insert_count.inc_comment();
                        insert_count.inc_total();

                    }

                } else {

                    total.inc_syntax();
                    profile_stack.at(profile_stack.size() - 3)->is_syntax = true;
                    ++profile_stack.at(profile_stack.size() - 3)->syntax_count;
                    ++profile_stack.at(profile_stack.size() - 3)->total_count;

                    if(srcdiff_stack.back().is_change) {

                        change_count.inc_syntax();
                        change_count.inc_total();
                        
                    } else if(srcdiff_stack.back().operation == SRCDIFF_DELETE) {

                        delete_count.inc_syntax();
                        delete_count.inc_total();

                    } else {

                        insert_count.inc_syntax();
                        insert_count.inc_total();

                    }

                }

                total.inc_total();

            }

        }

    }

}

/**
 * endRoot
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of the root profile.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::endRoot(const char * localname, const char * prefix, const char * URI) {

    if(text != "") process_characters();        

}

/**
 * endUnit
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an unit.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::endUnit(const char * localname, const char * prefix, const char * URI) {

    if(text != "") process_characters();

    counting_profile_pos.pop_back();
    profile_list[profile_stack.back()->id] = profile_stack.back();

    profile_stack.pop_back();
    uri_stack.pop_back();

}

/**
 * endElement
 * @param localname the name of the profile tag
 * @param prefix the tag prefix
 * @param URI the namespace of tag
 *
 * SAX handler function for end of an profile.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::endElement(const char * localname, const char * prefix, const char * URI) {

    const std::string local_name(localname);

    // detect if interchange
    size_t srcml_depth = uri_stack.size();
    bool is_interchange = srcml_depth > 4 && uri_stack.at(srcml_depth - 4) == SRCDIFF && srcml_element_stack.at(srcml_depth - 4) == "diff:delete"
                            && uri_stack.at(srcml_depth - 3) == SRC && uri_stack.at(srcml_depth - 2) == SRCDIFF && srcml_element_stack.at(srcml_depth - 2) == "diff:insert";

    if(text != "") process_characters();

    if(uri_stack.back() == SRCDIFF) srcdiff_stack.pop_back();

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += local_name;

    if(uri_stack.back() != SRCDIFF) {

        if(local_name == "name") {

            --name_count;

            if(name_count == 0) {

                size_t parent_pos = profile_stack.size() - 2;
                while(parent_pos > 0 && profile_stack.at(parent_pos)->uri == SRCDIFF)
                    --parent_pos;

                profile_stack.at(counting_profile_pos.back().first)->set_name(collected_name, profile_stack.at(parent_pos)->type_name);
                collected_name.reset();

            }

        }

        if(!is_interchange) --srcdiff_stack.back().level;

    }

    if(profile_stack.back()->is_modified) {

        profile_stack.at(profile_stack.size() - 2)->is_modified = true;
        profile_stack.at(profile_stack.size() - 2)->modified_count += profile_stack.back()->modified_count;
        profile_stack.at(profile_stack.size() - 2)->total_count += profile_stack.back()->total_count;

        if(profile_stack.back()->is_whitespace) {

            profile_stack.at(profile_stack.size() - 2)->is_whitespace = true;
            profile_stack.at(profile_stack.size() - 2)->whitespace_count += profile_stack.back()->whitespace_count;

        }

        if(profile_stack.back()->is_comment) {

            profile_stack.at(profile_stack.size() - 2)->is_comment = true;
            profile_stack.at(profile_stack.size() - 2)->comment_count += profile_stack.back()->comment_count;

        }

        if(profile_stack.back()->is_syntax) {

            profile_stack.at(profile_stack.size() - 2)->is_syntax = true;
            profile_stack.at(profile_stack.size() - 2)->syntax_count += profile_stack.back()->syntax_count;

        }

    }

    if(uri_stack.back() != SRCDIFF && !is_interchange && is_count(full_name)) {

        counting_profile_pos.pop_back();

        profile_stack.at(counting_profile_pos.back().second)->inc_num_child_profiles();

        // do not save items with no changes and not inserted/deleted
        if(profile_stack.back()->total_count || srcdiff_stack.back().operation != SRCDIFF_COMMON) {

            if(profile_list.size() < profile_stack.back()->id)
                profile_list.resize(profile_stack.back()->id * 2);

            profile_list[profile_stack.back()->id] = profile_stack.back();

            // should always have at least unit
            profile_stack.at(counting_profile_pos.back().second)->add_child(profile_stack.back());

        }

    }

    if(!is_interchange) profile_stack.pop_back();
    uri_stack.pop_back();

}

/**
 * charactersRoot
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling at the root level.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::charactersRoot(const char * ch, int len) {}

/**
 * charactersUnit
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling within a unit.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::charactersUnit(const char * ch, int len) {

    if(len == 0) return;

    text.append(ch, len);

    if(name_count) collected_name.append(ch, len, srcdiff_stack.back().operation);

}
