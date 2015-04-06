#include <srcdiff_summary.hpp>

#include <type_query.hpp>

#include <unit_profile_t.hpp>
#include <function_profile_t.hpp>
#include <parameter_profile_t.hpp>
#include <decl_stmt_profile_t.hpp>
#include <conditional_profile_t.hpp>
#include <if_profile_t.hpp>
#include <expr_stmt_profile_t.hpp>
#include <identifier_profile_t.hpp>
#include <expr_profile_t.hpp>

#include <cstring>
#include <algorithm>
#include <sys/ioctl.h>

bool is_count(const std::string & type_name) {

return is_function_type(type_name)  || is_class_type(type_name)           || is_simple_type(type_name)
    || is_condition_type(type_name) || is_catch(type_name)                || is_decl_stmt(type_name)
    || is_call(type_name)           || is_preprocessor_special(type_name) || is_expr(type_name)
    || is_template(type_name)       || is_parameter(type_name)            || is_lambda(type_name)
    || is_specifier(type_name)      || is_expr_stmt(type_name)            || is_argument(type_name)
    || is_comment(type_name)        || is_emit(type_name);

}

bool is_summary(const std::string & type_name) {

    return /*is_class_type(type_name) || */is_function_type(type_name);

}

std::shared_ptr<profile_t> make_profile(const std::string & type_name, namespace_uri uri, srcdiff_type operation,
                                        const std::shared_ptr<profile_t> & parent) {

    if(is_identifier(type_name))     return std::make_shared<identifier_profile_t> (type_name, uri, operation, parent);
    if(is_class_type(type_name))     return std::make_shared<class_profile_t>      (type_name, uri, operation, parent);
    if(is_function_type(type_name))  return std::make_shared<function_profile_t>   (type_name, uri, operation, parent);
    if(is_parameter(type_name))      return std::make_shared<parameter_profile_t>  (type_name, uri, operation, parent);
    if(is_decl_stmt(type_name))      return std::make_shared<decl_stmt_profile_t>  (type_name, uri, operation, parent);
    if(has_then_clause(type_name))   return std::make_shared<if_profile_t>         (type_name, uri, operation, parent);
    if(is_condition_type(type_name)) return std::make_shared<conditional_profile_t>(type_name, uri, operation, parent);
    if(is_call(type_name))           return std::make_shared<call_profile_t>       (type_name, uri, operation, parent);
    if(is_expr_stmt(type_name))      return std::make_shared<expr_stmt_profile_t>  (type_name, uri, operation, parent);
    if(is_expr(type_name))           return std::make_shared<expr_profile_t>       (type_name, uri, operation, parent);
    return std::make_shared<profile_t>                                             (type_name, uri, operation, parent);

}

void srcdiff_summary::process_characters() {

    if(text.empty()) return;

    if(profile_stack.back()->type_name.first_active_string() == "operator") {

        size_t expr_pos = profile_stack.size() - 2;
        while(expr_pos > 0 && !is_expr(profile_stack.at(expr_pos)->type_name))
            --expr_pos;

        if(expr_pos == 0) goto no_expr;

        if(text[0] == '=' && (text.size() == 1 || text.back() != '=')) {

            reinterpret_cast<std::shared_ptr<expr_profile_t> &>(profile_stack.at(expr_pos))->assignment(true);

            if(expr_stmt_pos > 0 && (expr_pos - 1) == expr_stmt_pos) {

                std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(profile_stack.at(expr_stmt_pos));
                expr_stmt_profile->assignment(true);

                for(size_t pos = text.size(); pos > 0; --pos) {

                    if(collect_lhs.has_original() && !collect_lhs.original().empty() && collect_lhs.original().back() == text[pos - 1])
                        collect_lhs.original().pop_back();

                    if(collect_lhs.has_modified() && !collect_lhs.modified().empty() && collect_lhs.modified().back() == text[pos - 1])
                        collect_lhs.modified().pop_back();
                }

                while(collect_lhs.has_original() && !collect_lhs.original().empty() && isspace(collect_lhs.original().back()))
                    collect_lhs.original().pop_back();

                while(collect_lhs.has_modified() && !collect_lhs.modified().empty() && isspace(collect_lhs.modified().back()))
                    collect_lhs.modified().pop_back();

                expr_stmt_profile->lhs(collect_lhs);
                collect_lhs.clear();


            }

        } else if(text == "delete") {

            reinterpret_cast<std::shared_ptr<expr_profile_t> &>(profile_stack.at(expr_pos))->is_delete(true);

            if(expr_stmt_pos > 0 && (expr_pos - 1) == expr_stmt_pos)
                reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(profile_stack.at(expr_stmt_pos))->is_delete(true);

        } else if(expr_stmt_pos > 0) {

            if(text != "." && text != "->" && text != ".*" && text != "->*")
                reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(profile_stack.at(expr_stmt_pos))->call(false);

        }

    }

no_expr:

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

        }


    }

    text.clear();

}

srcdiff_summary::srcdiff_summary(const std::string & output_filename, const boost::optional<std::string> & summary_type_str) 
    : out(nullptr), summary_types(summary_type::NONE), id_count(0), unit_profile(),
      srcdiff_stack(), profile_stack(), counting_profile_pos(), expr_stmt_pos(0), function_pos(0),
      insert_count(), delete_count(), change_count(), total(),
      text(), name_count(0), collected_name(), condition_count(0), collected_condition(), collect_lhs() {

    if(output_filename != "-")
      out = new std::ofstream(output_filename.c_str());
    else
      out = &std::cout;

    if(!summary_type_str || (*summary_type_str)[0] == '\0') return;

    std::string summary_type_str_copy = *summary_type_str;
    while(!summary_type_str_copy.empty()) {

        std::string::size_type end_pos = summary_type_str_copy.find(',');

        std::string type_str = summary_type_str_copy.substr(0, end_pos);
        if(type_str == "text")       summary_types |= summary_type::TEXT;
        else if(type_str == "table") summary_types |= summary_type::TABLE;

        summary_type_str_copy.erase(0, end_pos == std::string::npos ? end_pos : end_pos + 1);

    }

}

srcdiff_summary::~srcdiff_summary() {

    if(out != &std::cout) {

      ((std::ofstream *)out)->close();
      delete out;

    }

}

void srcdiff_summary::summarize(const std::string & srcdiff, const std::string & xml_encoding) {

    srcSAXController controller(srcdiff, xml_encoding.c_str());

    controller.parse(this);

    static bool first = true;
    if(!first) (*out) << '\n';
    else first = false;
    
    summarize(unit_profile);

    reset();

}

void srcdiff_summary::summarize(const std::shared_ptr<profile_t> & profile) {

	if(profile->total_count == 0) return;

    const char * columns_str = std::getenv("COLUMNS");

    size_t columns = 0;
    if(columns_str)
        columns = std::stoull(columns_str);

    if(columns == 0) {

        struct winsize win_size;
        ioctl(0, TIOCGWINSZ, &win_size);
        columns = win_size.ws_col;

    }

    summary_output_stream output_stream(*out);
 
    profile->summary(output_stream, summary_types);
    (*out) << "\n";

}

void srcdiff_summary::reset() {

    id_count = 0;
    srcdiff_stack.clear();
    profile_stack.clear();
    counting_profile_pos.clear();
    expr_stmt_pos = 0;
    function_pos = 0;
    text.clear();
    name_count = 0;
    collected_name.clear();
    condition_count = 0;
    collected_condition.clear();


}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behaviour.
 */
void srcdiff_summary::startDocument() {}

/**
 * endDocument
 *
 * SAX handler function for end of document.
 * Overide for desired behaviour.
 */
void srcdiff_summary::endDocument() {}

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
void srcdiff_summary::startRoot(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    process_characters();

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
void srcdiff_summary::startUnit(const char * localname, const char * prefix, const char * URI,
                       int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                       const struct srcsax_attribute * attributes) {

    uri_stack.push_back(SRC);
    srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, false, false));

    process_characters();

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += localname;

    unit_profile = std::make_shared<unit_profile_t>(full_name, SRC, SRCDIFF_COMMON);
    profile_stack.push_back(unit_profile);

    for(int i = 0; i < num_attributes; ++i)
        if(attributes[i].localname == std::string("filename")) {

            std::string file_name = attributes[i].value;

            std::string::size_type pos = file_name.find('|');

            if(pos == std::string::npos) profile_stack.back()->set_name(versioned_string(file_name));
            else profile_stack.back()->set_name(versioned_string(file_name.substr(0, pos), file_name.substr(pos + 1)));

            break;

        }

    counting_profile_pos.emplace_back(profile_stack.size() - 1, profile_stack.size() - 1, profile_stack.size() - 1);
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
void srcdiff_summary::startElement(const char * localname, const char * prefix, const char * URI,
                            int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                            const struct srcsax_attribute * attributes) {

    process_characters();

    const std::string local_name(localname);

    std::string full_name = "";
    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }
    full_name += local_name;

    uri_stack.push_back(URI == std::string("http://www.sdml.info/srcDiff") ? SRCDIFF : (URI == std::string("http://www.sdml.info/srcML/src") ? SRC : CPP));

    // detect if interchange
    size_t srcml_depth = uri_stack.size();
    bool is_interchange = srcml_depth > 4 && uri_stack.at(srcml_depth - 4) == SRCDIFF && srcml_element_stack.at(srcml_depth - 4) == "diff:delete"
                            && uri_stack.at(srcml_depth - 3) == SRC && uri_stack.at(srcml_depth - 2) == SRCDIFF && srcml_element_stack.at(srcml_depth - 2) == "diff:insert";

    bool then_clause_child = profile_stack.size() > 1
        && (profile_stack.back()->type_name == "then" || profile_stack.at(profile_stack.size() - 2)->type_name == "then")
        && has_then_clause(profile_stack.at(std::get<0>(counting_profile_pos.back()))->type_name);
    if(then_clause_child && full_name != "block" && full_name != "comment" && full_name != "break" && full_name != "continue" && full_name != "return")
        reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.back())))->is_guard(false);

    if(uri_stack.back() == SRCDIFF) {

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

        if(srcdiff_stack.back().operation == SRCDIFF_DELETE || srcdiff_stack.back().operation == SRCDIFF_INSERT)
            profile_stack.back()->is_modified = true;

    }

    if(is_interchange) {

        // update element name/operation
        profile_stack.at(std::get<0>(counting_profile_pos.back()))->set_operation(SRCDIFF_COMMON);
        profile_stack.at(std::get<0>(counting_profile_pos.back()))->type_name.set_modified(full_name);

        // correct element counts
        std::shared_ptr<profile_t> & parent = profile_stack.at(std::get<0>(counting_profile_pos.back()) - 2);
        size_t syntax_dec = 1;
        parent->syntax_count -= syntax_dec;
        parent->total_count  -= syntax_dec;
        if(parent->syntax_count == 0) parent->is_syntax = false;

        // correct global counts
        delete_count.syntax -= syntax_dec;
        delete_count.total  -= syntax_dec;
        total.syntax        -= syntax_dec;
        total.total         -= syntax_dec;

    } else {

        profile_stack.push_back(make_profile(full_name, uri_stack.back(), srcdiff_stack.back().operation, profile_stack.at(std::get<0>(counting_profile_pos.back()))));
        if(srcdiff_stack.back().is_change) profile_stack.back()->is_replacement = true;

    }

    if(uri_stack.back() != SRCDIFF) {

        if(full_name == "name")
            ++name_count;
        else if(full_name == "condition")
            ++condition_count;
        else if(full_name == "expr_stmt")
            expr_stmt_pos = profile_stack.size() - 1;
        else if(is_function_type(full_name))
            function_pos = profile_stack.size() - 1;

        if(!is_interchange) ++srcdiff_stack.back().level;

        if(!is_interchange && (is_count(full_name) || (is_identifier(full_name) && name_count == 1))) {

            bool summarize = is_summary(full_name);
            bool a_body    = has_body(full_name);
            counting_profile_pos.emplace_back(profile_stack.size() - 1, 
                                              a_body    ? profile_stack.size() - 1 : std::get<1>(counting_profile_pos.back()),
                                              summarize ? profile_stack.size() - 1 : std::get<2>(counting_profile_pos.back()));
            profile_stack.back()->set_id(++id_count);

        }

        if(srcdiff_stack.back().operation != SRCDIFF_COMMON) {

            if(srcdiff_stack.back().level == 1) {

                if(full_name == "comment") {

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
void srcdiff_summary::endRoot(const char * localname, const char * prefix, const char * URI) {

    process_characters();        

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
void srcdiff_summary::endUnit(const char * localname, const char * prefix, const char * URI) {

    process_characters();

    counting_profile_pos.pop_back();

    profile_stack.pop_back();
    uri_stack.pop_back();

}

void srcdiff_summary::update_anscestor_profile(const std::shared_ptr<profile_t> & profile) {

    size_t parent_pos = profile_stack.size() - 2;
    while(parent_pos > 0 && profile_stack.at(parent_pos)->uri == SRCDIFF)
        --parent_pos;

    // should always have at least unit
    profile_stack.at(std::get<0>(counting_profile_pos.back()))->add_child(profile, profile_stack.at(parent_pos)->type_name);
    profile_stack.at(std::get<2>(counting_profile_pos.back()))->add_descendant(profile, profile_stack.at(parent_pos)->type_name);

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
void srcdiff_summary::endElement(const char * localname, const char * prefix, const char * URI) {

    process_characters();

    const std::string local_name(localname);

    // detect if interchange
    size_t srcml_depth = uri_stack.size();
    bool is_interchange = srcml_depth > 4 && uri_stack.at(srcml_depth - 4) == SRCDIFF && srcml_element_stack.at(srcml_depth - 4) == "diff:delete"
                            && uri_stack.at(srcml_depth - 3) == SRC && uri_stack.at(srcml_depth - 2) == SRCDIFF && srcml_element_stack.at(srcml_depth - 2) == "diff:insert";

    if(uri_stack.back() == SRCDIFF) srcdiff_stack.pop_back();

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += local_name;

    if(uri_stack.back() != SRCDIFF) {

        if(!is_interchange) --srcdiff_stack.back().level;

        if(full_name == "name") {

            --name_count;

            if(name_count == 0) {

                size_t parent_pos = profile_stack.size() - 2;
                while(parent_pos > 0 && profile_stack.at(parent_pos)->uri == SRCDIFF)
                    --parent_pos;

                // set identifier_profile_t name
                profile_stack.at(std::get<0>(counting_profile_pos.back()))->set_name(collected_name, profile_stack.at(parent_pos)->type_name);

                // set name of identifiers parent profile
                profile_stack.at(std::get<0>(counting_profile_pos.at(counting_profile_pos.size() - 2)))->set_name(collected_name, profile_stack.at(parent_pos)->type_name);

                if(srcdiff_stack.back().operation != SRCDIFF_COMMON || !collected_name.is_common())
                    profile_stack.at(std::get<1>(counting_profile_pos.back()))->add_identifier(collected_name);

                collected_name.clear();

            }

        } else if(full_name == "condition") {

            --condition_count;

            if(condition_count == 0) {

                if(collected_condition.has_original() && !collected_condition.original().empty() && collected_condition.original()[0] == '(')
                    collected_condition.original().erase(collected_condition.original().begin());
                if(collected_condition.has_original() && !collected_condition.original().empty() && collected_condition.original().back() == ')')
                    collected_condition.original().pop_back();

                if(collected_condition.has_modified() && !collected_condition.modified().empty() && collected_condition.modified()[0] == '(')
                    collected_condition.modified().erase(collected_condition.modified().begin());
                if(collected_condition.has_modified() && !collected_condition.modified().empty() && collected_condition.modified().back() == ')')
                    collected_condition.modified().pop_back();

                reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.back())))->set_condition(collected_condition);

            }

            if(profile_stack.back()->total_count > 0)
                reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.back())))->set_condition_modified(true);

        } else if(full_name == "expr_stmt") {

            expr_stmt_pos = 0;
            collect_lhs.clear();

        } else if(full_name == "argument_list" && is_call(profile_stack.at(profile_stack.size() - 2)->type_name.first_active_string())
                  && profile_stack.back()->total_count > 0) {

            reinterpret_cast<std::shared_ptr<call_profile_t> &>(profile_stack.at(profile_stack.size() - 2))->argument_list_modified = true;

        } else if(full_name == "block" && profile_stack.back()->total_count > 0 && (profile_stack.at(profile_stack.size() - 2)->type_name == "then"
                                        || is_condition_type(profile_stack.at(profile_stack.size() - 2)->type_name.first_active_string()))) {

            reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.back())))->set_body_modified(true);

        } else if(full_name == "else" && counting_profile_pos.size() > 1
            && profile_stack.at(std::get<0>(counting_profile_pos.at(counting_profile_pos.size() - 2)))->type_name == "if") {

            reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.at(counting_profile_pos.size() - 2))))->else_clause(true);
            reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.at(counting_profile_pos.size() - 2))))->is_guard(false);

            if(profile_stack.back()->operation != SRCDIFF_COMMON || profile_stack.back()->syntax_count != 0)
                reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(std::get<0>(counting_profile_pos.at(counting_profile_pos.size() - 2))))->else_operation(profile_stack.back()->operation);
    
        } else if(is_function_type(full_name)) {

            function_pos = 0;

        }


    }

    // inner expr_stmt handling
    if(expr_stmt_pos > 0 && ((profile_stack.size() - 1) - expr_stmt_pos) == 2) {

        std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(profile_stack.at(expr_stmt_pos));

        if(!is_call(full_name) && full_name != "operator")
            expr_stmt_profile->call(false);

        if(is_call(full_name))
            expr_stmt_profile->add_call_profile(reinterpret_cast<std::shared_ptr<call_profile_t> &>(profile_stack.back()));


    }

    if(!is_interchange && profile_stack.back()->is_modified) {

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

    if(is_identifier(full_name) && name_count == 0) {

        std::shared_ptr<identifier_profile_t> & identifier_profile = reinterpret_cast<std::shared_ptr<identifier_profile_t> &>(profile_stack.back());

        if(identifier_profile->operation != SRCDIFF_COMMON && identifier_profile->name.has_original() && identifier_profile->name.has_modified())
            identifier_profile->operation = SRCDIFF_COMMON;

    }

    if(uri_stack.back() != SRCDIFF && !is_interchange && (is_count(full_name) || (is_identifier(full_name) && name_count == 0))) {

        counting_profile_pos.pop_back();

        // do not save items with no changes and not inserted/deleted
        if(profile_stack.back()->total_count
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && srcdiff_stack.back().level == 0))
            update_anscestor_profile(profile_stack.back());
        /** @todo may want this even if total_count or syntax_count are not 0 */
        else if(profile_stack.back()->total_count == 0 && srcdiff_stack.back().operation == SRCDIFF_COMMON)
                profile_stack.at(std::get<0>(counting_profile_pos.back()))->add_common(profile_stack.back());

        if(has_body(full_name)) {

            std::shared_ptr<profile_t> & parent_body_profile = profile_stack.at(std::get<1>(counting_profile_pos.back()));

            // add to identifier list looking for intersections and adding
            for(std::pair<versioned_string, size_t> identifier : profile_stack.back()->identifiers) {

                std::map<versioned_string, size_t>::iterator itr = parent_body_profile->identifiers.find(identifier.first);
                if(itr == parent_body_profile->identifiers.end()) {

                    parent_body_profile->identifiers.insert(itr, identifier);

                } else {

                    itr->second += identifier.second;

                    std::map<versioned_string, size_t>::iterator itersect_itr = parent_body_profile->summary_identifiers.find(itr->first);
                    if(itersect_itr == parent_body_profile->summary_identifiers.end())
                        parent_body_profile->summary_identifiers.insert(itersect_itr, *itr);
                    else
                        itersect_itr->second += itr->second;

                }

            }

            for(std::map<versioned_string, size_t>::iterator itr = profile_stack.back()->summary_identifiers.begin();
                itr != profile_stack.back()->summary_identifiers.end();) {
    
                if(itr->second <= 1) 
                   profile_stack.back()->summary_identifiers.erase(itr++);
               else
                ++itr;

           }

        }

    }

    if(is_statement(full_name)) {

        if(function_pos)
            ++profile_stack.at(function_pos)->statement_count;

        size_t parent_pos = std::get<0>(counting_profile_pos.back());
        if(parent_pos != function_pos)
            ++profile_stack.at(parent_pos)->statement_count;

       if(profile_stack.back()->operation != SRCDIFF_COMMON || profile_stack.back()->syntax_count) {

        bool condition_change = false;
        if(is_condition_type(full_name))
            condition_change = reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.back())->is_condition_modified();

        size_t increment_amount = 1;
        if(condition_change) increment_amount = 2;

        if(function_pos)
            profile_stack.at(function_pos)->statement_churn += increment_amount;

        if(parent_pos != function_pos)
            profile_stack.at(parent_pos)->statement_churn += increment_amount;        

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
void srcdiff_summary::charactersRoot(const char * ch, int len) {}

/**
 * charactersUnit
 * @param ch the characers
 * @param len number of characters
 *
 * SAX handler function for character handling within a unit.
 * Overide for desired behaviour.
 */
void srcdiff_summary::charactersUnit(const char * ch, int len) {

    if(len == 0) return;

    text.append(ch, len);

    if(name_count) collected_name.append(ch, len, srcdiff_stack.back().operation);
    if(condition_count) collected_condition.append(ch, len, srcdiff_stack.back().operation);
    if(expr_stmt_pos) collect_lhs.append(ch, len, srcdiff_stack.back().operation);



}
