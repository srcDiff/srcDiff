#include <srcdiff_summary.hpp>

#include <srcdiff_constants.hpp>

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
#include <ternary_profile_t.hpp>
#include <exception_profile_t.hpp>
#include <interchange_profile_t.hpp>

#include <cstring>
#include <algorithm>
#include <sys/ioctl.h>

static bool is_count(const std::string & type_name) {

return is_function_type(type_name)  || is_class_type(type_name)           || is_simple_type(type_name)
    || is_condition_type(type_name) || is_catch(type_name)                || is_decl_stmt(type_name)
    || is_call(type_name)           || is_preprocessor_special(type_name) || is_expr(type_name)
    || is_template(type_name)       || is_parameter(type_name)            || is_lambda(type_name)
    || is_specifier(type_name)      || is_expr_stmt(type_name)            || is_argument(type_name)
    || is_comment(type_name)        || is_emit(type_name)                 || is_jump(type_name)
    || is_ternary(type_name)        || is_label(type_name)                || is_init(type_name)
    || is_type(type_name)           || is_expr_block(type_name)           || is_interchange(type_name);

}

static bool is_summary(const std::string & type_name) {

    return is_function_type(type_name) || is_class_type(type_name);

}

static std::shared_ptr<profile_t> make_profile(const std::string & type_name, namespace_uri uri, srcdiff_type operation,
                                        const std::shared_ptr<profile_t> & summary_parent) {

    if(is_identifier(type_name))         return std::make_shared<identifier_profile_t> (type_name, uri, operation, summary_parent);
    if(is_class_type(type_name))         return std::make_shared<class_profile_t>      (type_name, uri, operation, summary_parent);
    if(is_function_type(type_name))      return std::make_shared<function_profile_t>   (type_name, uri, operation, summary_parent);
    if(is_parameter(type_name))          return std::make_shared<parameter_profile_t>  (type_name, uri, operation, summary_parent);
    if(is_decl_stmt(type_name))          return std::make_shared<decl_stmt_profile_t>  (type_name, uri, operation, summary_parent);
    if(is_if(type_name))                 return std::make_shared<if_profile_t>         (type_name, uri, operation, summary_parent);
    if(is_ternary(type_name))            return std::make_shared<ternary_profile_t>    (type_name, uri, operation, summary_parent);
    if(is_condition_type(type_name))     return std::make_shared<conditional_profile_t>(type_name, uri, operation, summary_parent);
    if(is_call(type_name))               return std::make_shared<call_profile_t>       (type_name, uri, operation, summary_parent);
    if(is_expr_stmt(type_name))          return std::make_shared<expr_stmt_profile_t>  (type_name, uri, operation, summary_parent);
    if(is_expr(type_name))               return std::make_shared<expr_profile_t>       (type_name, uri, operation, summary_parent);
    if(is_exception_handling(type_name)) return std::make_shared<exception_profile_t>  (type_name, uri, operation, summary_parent);
    return std::make_shared<profile_t>                                                 (type_name, uri, operation, summary_parent);

}

static std::shared_ptr<profile_t> & get_stmt_from_interchange(std::shared_ptr<profile_t> & profile, std::string statement) {

    if(!is_interchange(profile->type_name)) return profile;

    std::shared_ptr<interchange_profile_t> & interchange_profile = reinterpret_cast<std::shared_ptr<interchange_profile_t> &>(profile);

    if(interchange_profile->original()->type_name == statement) return interchange_profile->original();
    return interchange_profile->modified();

}

void srcdiff_summary::process_characters() {

    if(text.empty()) return;

    if(profile_stack.back()->type_name.first_active_string() == "operator") {

        size_t expr_pos = profile_stack.size() - 2;
        while(expr_pos > 0 && !is_expr(profile_stack.at(expr_pos)->type_name))
            --expr_pos;

        if(expr_pos == 0) goto no_expr;

        /** @todo need to make sure this actually works with complex deletes of different portions. */
        if(text[0] == '=' && (text.size() == 1 || text.back() != '=') && (srcdiff_stack.back().operation == SRCDIFF_COMMON || profile_stack.at(expr_pos)->operation != SRCDIFF_COMMON)) {

            reinterpret_cast<std::shared_ptr<expr_profile_t> &>(profile_stack.at(expr_pos))->assignment(true);

            if(expr_stmt_pos.size() > 0 && (expr_pos - 1) == expr_stmt_pos.back()) {

                std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(expr_stmt_pos.back()), "expr_stmt"));

                expr_stmt_profile->assignment(true);

                for(size_t pos = text.size(); pos > 0; --pos) {

                    if(collect_lhs.back().has_original() && !collect_lhs.back().original().empty() && collect_lhs.back().original().back() == text[pos - 1])
                        collect_lhs.back().original().pop_back();

                    if(collect_lhs.back().has_modified() && !collect_lhs.back().modified().empty() && collect_lhs.back().modified().back() == text[pos - 1])
                        collect_lhs.back().modified().pop_back();
                }

                while(collect_lhs.back().has_original() && !collect_lhs.back().original().empty() && isspace(collect_lhs.back().original().back()))
                    collect_lhs.back().original().pop_back();

                while(collect_lhs.back().has_modified() && !collect_lhs.back().modified().empty() && isspace(collect_lhs.back().modified().back()))
                    collect_lhs.back().modified().pop_back();

                expr_stmt_profile->lhs(collect_lhs.back());
                left_hand_side.back() = false;


            }

        } else if(text == "delete") {

            reinterpret_cast<std::shared_ptr<expr_profile_t> &>(profile_stack.at(expr_pos))->is_delete(true);

            if(expr_stmt_pos.size() > 0 && (expr_pos - 1) == expr_stmt_pos.back()) {

                std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(expr_stmt_pos.back()), "expr_stmt"));
                expr_stmt_profile->is_delete(true);

            }

        }

        if(expr_stmt_pos.size() > 0 && (expr_pos - 1) == expr_stmt_pos.back()) {

            std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(expr_stmt_pos.back()), "expr_stmt"));

            if(text == "<<" && !expr_stmt_profile->assignment())
                expr_stmt_profile->print(true);

            if(text != "." && text != "->" && text != ".*" && text != "->*" && text != "::")
                expr_stmt_profile->call(false);

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

                if(!isspace((char)*current_ch))
                    break;

            }

            if(current_ch == save_ch) {

                is_whitespace = false;

                for(; current_ch < (ch + len); ++current_ch) {

                    if(isspace((char)*current_ch))
                        break;

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
    : out(nullptr), summary_types(summary_type::NONE), id_count(0),
      srcdiff_stack(), profile_stack(), counting_profile_pos(), expr_stmt_pos(), function_pos(), current_move_id(0),
      insert_count(), delete_count(), change_count(), total(),
      text(), specifier_raw(), name_count(0), collected_full_name(), collected_simple_name(), simple_names(),
      condition_count(0), collected_condition(), left_hand_side(), collect_lhs(), collect_rhs(), raw_statements() {

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
    
    summarize(profile_t::unit_profile);

    reset();

}

void srcdiff_summary::summarize(const std::shared_ptr<profile_t> & profile) {

    if(profile->total_count == 0) return;

    const char * columns_str = std::getenv("COLUMNS");

    size_t columns = 0;
    if(columns_str) columns = std::stoull(columns_str);

    if(columns == 0) {

        struct winsize win_size;
        ioctl(0, TIOCGWINSZ, &win_size);
        columns = win_size.ws_col;

    }

    summary_output_stream output_stream(*out, columns);
 
    profile->summary(output_stream, summary_types);
    (*out) << "\n";

}

void srcdiff_summary::reset() {

    id_count = 0;
    srcdiff_stack.clear();
    profile_stack.clear();
    counting_profile_pos.clear();
    expr_stmt_pos.clear();
    function_pos.clear();
    current_move_id = 0;
    text.clear();
    specifier_raw.clear();
    name_count = 0;
    collected_full_name.clear();
    collected_simple_name.clear();
    simple_names.clear();
    condition_count = 0;
    collected_condition.clear();
    left_hand_side.clear();
    collect_lhs.clear();
    collect_rhs.clear();

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

    profile_stack.push_back(profile_t::unit_profile = std::make_shared<unit_profile_t>(full_name, SRC, SRCDIFF_COMMON));
    profile_stack.back()->parent = profile_stack.back();
    profile_stack.back()->body = profile_stack.back();
    profile_stack.back()->summary_profile = profile_stack.back();

    for(int i = 0; i < num_attributes; ++i)
        if(attributes[i].localname == std::string("filename")) {

            std::string file_name = attributes[i].value;

            std::string::size_type pos = file_name.find('|');

            std::shared_ptr<unit_profile_t> & unit_profile = reinterpret_cast<std::shared_ptr<unit_profile_t> &>(profile_t::unit_profile);
            if(pos == std::string::npos) {

                unit_profile->file_name = versioned_string(file_name);
                unit_profile->operation = SRCDIFF_COMMON;
            
            } else { 

                unit_profile->file_name = versioned_string(file_name.substr(0, pos), file_name.substr(pos + 1));
                unit_profile->operation = !unit_profile->file_name.has_original() ? SRCDIFF_INSERT 
                                            : (!unit_profile->file_name.has_modified() ? SRCDIFF_DELETE : SRCDIFF_COMMON);

            }

            break;

        }

    counting_profile_pos.emplace_back(0);
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

    if(is_block(full_name) && (is_block(profile_stack.back()->type_name) || (profile_stack.back()->uri == SRCDIFF && is_block(profile_stack.at(profile_stack.size() - 2)->type_name))))
        full_name = "expr_block";

    uri_stack.push_back(URI == SRCDIFF_DEFAULT_NAMESPACE_HREF ? SRCDIFF : (URI == SRCML_SRC_NAMESPACE_HREF ? SRC : CPP));

    // detect if interchange
    size_t srcml_depth = uri_stack.size();
    bool is_interchange = srcml_depth > 3 && uri_stack.at(srcml_depth - 4) == SRCDIFF && srcml_element_stack.at(srcml_depth - 4) == "diff:delete"
                            && uri_stack.at(srcml_depth - 3) == SRC && uri_stack.at(srcml_depth - 2) == SRCDIFF && srcml_element_stack.at(srcml_depth - 2) == "diff:insert";

    bool then_clause_child = profile_stack.size() > 1
        && (profile_stack.back()->type_name == "then" || profile_stack.at(profile_stack.size() - 2)->type_name == "then")
        && is_if(profile_stack.at(counting_profile_pos.back())->type_name);
    if(then_clause_child && full_name != "block" && full_name != "comment" && full_name != "break" && full_name != "continue" && full_name != "return")
        reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(counting_profile_pos.back()))->is_guard(false);

    if(uri_stack.back() == SRCDIFF) {

        bool is_change = false;
        size_t move_id = 0;
        for(int i = 0; i < num_attributes; ++i) {

            if(attributes[i].localname == std::string("type")) {

                std::string value = attributes[i].value;
                if(value == "change") is_change = true;

            } else if(attributes[i].localname == std::string("move")) {

                move_id = std::stoull(attributes[i].value);

            }

        }

        if(local_name == "common")
            srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, is_change, move_id));
        else if(local_name == "delete")
            srcdiff_stack.push_back(srcdiff(SRCDIFF_DELETE, is_change, move_id));
        else if(local_name == "insert")
            srcdiff_stack.push_back(srcdiff(SRCDIFF_INSERT, is_change, move_id));

        bool is_interchange_diff = (srcml_depth > 3 && uri_stack.at(srcml_depth - 3) == SRCDIFF && srcml_element_stack.at(srcml_depth - 3) == "diff:delete"
                            && uri_stack.at(srcml_depth - 2) == SRC && uri_stack.back() == SRCDIFF && local_name == "insert");

        if(srcdiff_stack.back().operation == SRCDIFF_DELETE || srcdiff_stack.back().operation == SRCDIFF_INSERT)
            profile_stack.back()->is_modified = true;

    }

    if(is_interchange) {

        std::shared_ptr<profile_t> interchange_profile = std::make_shared<interchange_profile_t>("interchange", SRC, SRCDIFF_COMMON, profile_stack.at(profile_stack.size() - 2)->summary_parent);
        interchange_profile->parent = profile_stack.at(profile_stack.size() - 2)->parent;
        interchange_profile->body = profile_stack.at(profile_stack.size() - 2)->body;
        interchange_profile->summary_profile = profile_stack.at(profile_stack.size() - 2)->summary_profile;
        reinterpret_cast<std::shared_ptr<interchange_profile_t> &>(interchange_profile)->original() = profile_stack.at(profile_stack.size() - 2);
        reinterpret_cast<std::shared_ptr<interchange_profile_t> &>(interchange_profile)->modified() = make_profile(full_name, uri_stack.back(), srcdiff_stack.back().operation, interchange_profile->summary_parent);
        reinterpret_cast<std::shared_ptr<interchange_profile_t> &>(interchange_profile)->modified()->parent = interchange_profile->parent;
        reinterpret_cast<std::shared_ptr<interchange_profile_t> &>(interchange_profile)->modified()->body = interchange_profile->body;
        reinterpret_cast<std::shared_ptr<interchange_profile_t> &>(interchange_profile)->modified()->summary_profile = interchange_profile->summary_profile;

        // update element name/operation
        profile_stack.at(profile_stack.size() - 2) = interchange_profile;

        // correct element counts
        std::shared_ptr<profile_t> & parent = profile_stack.at(profile_stack.size() - 3);
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

        profile_stack.emplace_back(make_profile(full_name, uri_stack.back(), srcdiff_stack.back().operation, profile_stack.at(counting_profile_pos.back())));

        size_t parent_pos = profile_stack.size() - 2;
        while(parent_pos > 0 && profile_stack.at(parent_pos)->uri == SRCDIFF)
            --parent_pos;

        profile_stack.back()->parent = profile_stack.at(parent_pos);
        if(has_body(full_name))
            profile_stack.back()->body = profile_stack.back();
        else
            profile_stack.back()->body = profile_stack.back()->parent->body;

        if(is_summary(full_name))
            profile_stack.back()->summary_profile = profile_stack.back();
        else
            profile_stack.back()->summary_profile = profile_stack.back()->parent->summary_profile;

        if(srcdiff_stack.back().is_change) profile_stack.back()->is_replacement = true;
        if(srcdiff_stack.back().move_id && srcdiff_stack.back().level == 0 && uri_stack.back() != SRCDIFF && current_move_id < srcdiff_stack.back().move_id) {

            profile_stack.back()->move_id = srcdiff_stack.back().move_id;
            current_move_id = srcdiff_stack.back().move_id;

        }
        if(expr_stmt_pos.size() > 0) {

            if(left_hand_side.back()) profile_stack.back()->left_hand_side = true;
            else               profile_stack.back()->right_hand_side = true;

        }

    }

    if(uri_stack.back() != SRCDIFF) {

        if(is_identifier(profile_stack.back()->parent->type_name))
            reinterpret_cast<std::shared_ptr<identifier_profile_t> &>(profile_stack.back()->parent)->is_simple = false;

        if(is_identifier(full_name)) {

            ++name_count;
            collected_simple_name.clear();

        } else if(full_name == "condition") {

            ++condition_count;

        } else if(full_name == "expr_stmt") {

            expr_stmt_pos.emplace_back(is_interchange ? profile_stack.size() - 2 : profile_stack.size() - 1);
            left_hand_side.emplace_back(true);
            collect_lhs.emplace_back(versioned_string());
            collect_rhs.emplace_back(versioned_string());

        } else if(is_function_type(full_name)) {

            function_pos.emplace_back(profile_stack.size() - 1);

        }

        if(!is_interchange) ++srcdiff_stack.back().level;

        if(!is_interchange && (is_count(full_name) || (is_identifier(full_name) && name_count == 1))) {

            counting_profile_pos.push_back(profile_stack.size() - 1);
            profile_stack.back()->set_id(++id_count);

            if(is_statement(full_name) && srcdiff_stack.back().operation != SRCDIFF_COMMON) raw_statements.emplace(profile_stack.back()->id, profile_stack.back()->raw);

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
    profile_stack.at(counting_profile_pos.back())->add_child_change(profile, profile_stack.at(parent_pos)->type_name);
    profile_stack.back()->parent->summary_profile->add_descendant_change(profile, profile_stack.at(parent_pos)->type_name);

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
    bool is_interchange = srcml_depth > 3 && uri_stack.at(srcml_depth - 4) == SRCDIFF && srcml_element_stack.at(srcml_depth - 4) == "diff:delete"
                            && uri_stack.at(srcml_depth - 3) == SRC && uri_stack.at(srcml_depth - 2) == SRCDIFF && srcml_element_stack.at(srcml_depth - 2) == "diff:insert";

    if(uri_stack.back() == SRCDIFF) srcdiff_stack.pop_back();

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }
    full_name += local_name;

    if(is_block(full_name) && (is_block(profile_stack.at(profile_stack.size() - 2)->type_name) || (profile_stack.at(profile_stack.size() - 2)->uri == SRCDIFF && is_block(profile_stack.at(profile_stack.size() - 3)->type_name))))
        full_name = "expr_block";

    if(uri_stack.back() != SRCDIFF) {

        profile_stack.back()->parent->add_child(profile_stack.back());

        if(!is_interchange && srcdiff_stack.back().level > 0) --srcdiff_stack.back().level;

        if(is_identifier(full_name)) {

            --name_count;
            if(reinterpret_cast<std::shared_ptr<identifier_profile_t> &>(profile_stack.back())->is_simple)
                simple_names.emplace(collected_simple_name);
            collected_simple_name.clear();

            if(name_count == 0) {

                size_t parent_pos = profile_stack.size() - 2;
                while(parent_pos > 0 && profile_stack.at(parent_pos)->uri == SRCDIFF)
                    --parent_pos;

                // set identifier_profile_t name
                std::shared_ptr<identifier_profile_t> & identifier_profile = reinterpret_cast<std::shared_ptr<identifier_profile_t> &>(profile_stack.at(counting_profile_pos.back()));
                identifier_profile->name.swap(collected_full_name);
                identifier_profile->simple_names.swap(simple_names);

                // set name of identifiers parent profile
                profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2))->set_name(identifier_profile, profile_stack.at(parent_pos)->type_name);

                // add declaration to body
                profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2))->body->add_declaration_identifier(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)));

                std::shared_ptr<unit_profile_t> & unit_profile = reinterpret_cast<std::shared_ptr<unit_profile_t> &>(profile_t::unit_profile);

                for(const versioned_string & name : identifier_profile->simple_names) {

                    if(name.is_common()) {
    
                        std::map<std::string, std::vector<std::shared_ptr<profile_t>>>::iterator itr = unit_profile->identifier_to_declaration_profile.find(name);
                        if(itr != unit_profile->identifier_to_declaration_profile.end())
                            unit_profile->identifier_to_declaration_profile[name].back()->declarations[name].insert(name);

                        profile_stack.back()->body->identifiers[name][name].insert(identifier_profile->name);

                        continue;

                    }

                    if(name.has_original()) {

                        std::map<std::string, std::vector<std::shared_ptr<profile_t>>>::iterator itr = unit_profile->identifier_to_declaration_profile.find(name.original());
                        if(itr != unit_profile->identifier_to_declaration_profile.end())
                            unit_profile->identifier_to_declaration_profile[name.original()].back()->declarations[name.original()].insert(name);

                        profile_stack.back()->body->identifiers[name.original()][name].insert(identifier_profile->name);

                    }

                    if(name.has_modified()) {

                        std::map<std::string, std::vector<std::shared_ptr<profile_t>>>::iterator itr = unit_profile->identifier_to_declaration_profile.find(name.modified());
                        if(itr != unit_profile->identifier_to_declaration_profile.end())
                            unit_profile->identifier_to_declaration_profile[name.modified()].back()->declarations[name.modified()].insert(name);

                        profile_stack.back()->body->identifiers[name.modified()][name].insert(identifier_profile->name);

                    }

                }

                collected_full_name.clear();
                simple_names.clear();

            }

        }

        if(full_name == "condition") {

            --condition_count;

            if(is_ternary(profile_stack.at(counting_profile_pos.back())->type_name)) {

                reinterpret_cast<std::shared_ptr<ternary_profile_t> &>(profile_stack.at(counting_profile_pos.back()))->condition(profile_stack.back());

            } else {

                 if(condition_count == 0) {

                    if(collected_condition.has_original() && !collected_condition.original().empty() && collected_condition.original()[0] == '(')
                        collected_condition.original().erase(collected_condition.original().begin());
                    if(collected_condition.has_original() && !collected_condition.original().empty() && collected_condition.original().back() == ')')
                        collected_condition.original().pop_back();

                    if(collected_condition.has_modified() && !collected_condition.modified().empty() && collected_condition.modified()[0] == '(')
                        collected_condition.modified().erase(collected_condition.modified().begin());
                    if(collected_condition.has_modified() && !collected_condition.modified().empty() && collected_condition.modified().back() == ')')
                        collected_condition.modified().pop_back();

                    reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(counting_profile_pos.back()))->set_condition(collected_condition);
                    collected_condition.clear();

                }

                if(profile_stack.back()->syntax_count > 0)
                    reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(counting_profile_pos.back()))->set_condition_modified(true);

            }

        } else if(full_name == "then" && is_ternary(profile_stack.at(counting_profile_pos.back())->type_name)) {

            reinterpret_cast<std::shared_ptr<ternary_profile_t> &>(profile_stack.at(counting_profile_pos.back()))->then_clause(profile_stack.back());

        } else if(full_name == "expr_stmt") {

            if(left_hand_side.back()) reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(expr_stmt_pos.back()), "expr_stmt"))->lhs(collect_lhs.back());
            else                      reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(expr_stmt_pos.back()), "expr_stmt"))->rhs(collect_rhs.back());

            expr_stmt_pos.pop_back();
            left_hand_side.pop_back();
            collect_lhs.pop_back();
            collect_rhs.pop_back();


        } else if(full_name == "argument_list" && is_call(profile_stack.at(profile_stack.size() - 2)->type_name.first_active_string())
                  && profile_stack.back()->syntax_count > 0) {

            reinterpret_cast<std::shared_ptr<call_profile_t> &>(profile_stack.at(profile_stack.size() - 2))->argument_list_modified = true;

        } else if(full_name == "block" && profile_stack.back()->syntax_count > 0 && (profile_stack.at(profile_stack.size() - 2)->type_name == "then"
                                        || is_condition_type(profile_stack.at(counting_profile_pos.back())->type_name.first_active_string()))) {

            reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(counting_profile_pos.back()))->set_body_modified(true);

            if((counting_profile_pos.back() - 1) > 0 && profile_stack.at(counting_profile_pos.back() - 1)->type_name.first_active_string() == "elseif")
                reinterpret_cast<std::shared_ptr<conditional_profile_t> &>(profile_stack.at(counting_profile_pos.back() - 1))->set_body_modified(true);

        } else if((full_name == "else" || full_name == "elseif") && counting_profile_pos.size() > 1
            && is_if(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2))->type_name)) {

            if(full_name == "else") {

                reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)))->else_clause(true);
                if(profile_stack.back()->operation != SRCDIFF_COMMON || profile_stack.back()->syntax_count)
                    reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)))->else_operation(profile_stack.back()->operation);

            } else {

                reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)))->elseif_clause(true);
                if(profile_stack.back()->operation != SRCDIFF_COMMON || profile_stack.back()->syntax_count)
                    reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)))->elseif_operation(profile_stack.back()->operation);

            }

            reinterpret_cast<std::shared_ptr<if_profile_t> &>(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)))->is_guard(false);

        } else if(is_function_type(full_name)) {

            function_pos.pop_back();

        } else if(is_call(full_name)) {

             size_t expr_pos = profile_stack.size() - 2;
             while(expr_pos > 0 && !is_expr(profile_stack.at(expr_pos)->type_name) && profile_stack.at(expr_pos)->type_name != "member_init_list")
                --expr_pos;

            if(is_expr(profile_stack.at(expr_pos)->type_name))
                reinterpret_cast<std::shared_ptr<expr_profile_t> &>(profile_stack.at(expr_pos))->increment_calls();

        } else if(full_name == "catch" && counting_profile_pos.size() > 1
            && profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2))->type_name == "try") {

            reinterpret_cast<std::shared_ptr<exception_profile_t> &>(profile_stack.at(counting_profile_pos.at(counting_profile_pos.size() - 2)))->increment_catches();

        } else if(is_specifier(full_name)) {

            profile_stack.back()->raw = specifier_raw;
            specifier_raw.clear();

        } else if(profile_stack.size() > 2 && profile_stack.at(profile_stack.size() - 2)->type_name == "decl"
            && is_decl_stmt(profile_stack.at(profile_stack.size() - 3)->type_name)) {

            if(profile_stack.back()->type_name == "type")
                reinterpret_cast<std::shared_ptr<decl_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(profile_stack.size() - 3), "decl_stmt"))->type = profile_stack.back();
            else if(profile_stack.back()->type_name == "init")
                reinterpret_cast<std::shared_ptr<decl_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(profile_stack.size() - 3), "decl_stmt"))->init = profile_stack.back();

        } else if(profile_stack.size() > 2 && profile_stack.at(profile_stack.size() - 2)->type_name == "decl"
            && is_parameter(profile_stack.at(profile_stack.size() - 3)->type_name)) {

            if(profile_stack.back()->type_name == "type")
                reinterpret_cast<std::shared_ptr<parameter_profile_t> &>(profile_stack.at(profile_stack.size() - 3))->type = profile_stack.back();
            else if(profile_stack.back()->type_name == "init")
                reinterpret_cast<std::shared_ptr<parameter_profile_t> &>(profile_stack.at(profile_stack.size() - 3))->init = profile_stack.back();


        } else if(full_name == "type" && profile_stack.size() > 1 && is_function_type(profile_stack.at(profile_stack.size() - 2)->type_name)) {

                reinterpret_cast<std::shared_ptr<function_profile_t> &>(profile_stack.at(profile_stack.size() - 2))->return_type = profile_stack.back();

        }

    }

    // inner expr_stmt handling
    if(expr_stmt_pos.size() > 0 && ((profile_stack.size() - 1) - expr_stmt_pos.back()) == 2) {

        std::shared_ptr<expr_stmt_profile_t> & expr_stmt_profile = reinterpret_cast<std::shared_ptr<expr_stmt_profile_t> &>(get_stmt_from_interchange(profile_stack.at(expr_stmt_pos.back()), "expr_stmt"));

        if(!is_call(full_name) && uri_stack.back() != SRCDIFF && full_name != "operator")
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

        if(is_statement(full_name) && srcdiff_stack.back().operation != SRCDIFF_COMMON) raw_statements.erase(profile_stack.back()->id);

        counting_profile_pos.pop_back();

        // do not save items with no changes and not inserted/deleted
        if(profile_stack.back()->syntax_count > 0
            || (profile_stack.back()->total_count > 0 && profile_stack.size() == 2)
            || profile_stack.back()->move_id
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && srcdiff_stack.back().level == 0)
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && srcdiff_stack.back().level == 1 && full_name == "else")
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && srcdiff_stack.back().level == 1 && is_expr_block(profile_stack.at(profile_stack.size() - 2)->type_name))
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && full_name == "call" && profile_stack.at(profile_stack.size() - 2)->type_name == "member_init_list")
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && srcdiff_stack.back().level == 1 && profile_stack.at(profile_stack.size() - 2)->type_name == "expr")
            || (srcdiff_stack.back().operation != SRCDIFF_COMMON && full_name == "argument"))
            update_anscestor_profile(profile_stack.back());

        if(has_body(full_name)) {

            // sub body identifiers also part of parent.  Merge.
            std::shared_ptr<profile_t> & body_profile = profile_stack.back()->parent->body;

            for(std::map<std::string, std::map<versioned_string, std::multiset<versioned_string>>>::const_iterator top_citr = profile_stack.back()->identifiers.begin();
                top_citr != profile_stack.back()->identifiers.end(); ++top_citr) {

                std::map<std::string, std::map<versioned_string, std::multiset<versioned_string>>>::const_iterator pos_itr = body_profile->identifiers.find(top_citr->first);
                if(pos_itr == body_profile->identifiers.end()) {

                    body_profile->identifiers.insert(pos_itr, *top_citr);

                } else {

                    for(std::map<versioned_string, std::multiset<versioned_string>>::const_iterator middle_citr = top_citr->second.begin();
                        middle_citr != top_citr->second.end(); ++middle_citr) {

                        std::map<versioned_string, std::multiset<versioned_string>>::const_iterator middle_pos_citr = body_profile->identifiers[top_citr->first].find(middle_citr->first);
                        if(middle_pos_citr == body_profile->identifiers[top_citr->first].end()) {

                            body_profile->identifiers[top_citr->first].insert(middle_pos_citr, *middle_citr);

                        } else {

                            for(const versioned_string & use : middle_citr->second) {

                                body_profile->identifiers[top_citr->first][middle_citr->first].insert(use);

                            }

                        }

                    }

                }

            }

        }

    }

    if(profile_stack.back()->operation != SRCDIFF_COMMON && is_condition_type(full_name) && !is_ternary(full_name) && full_name != "elseif") {

        for(size_t pos : function_pos) {

            if(profile_stack.back()->operation == SRCDIFF_DELETE)
                reinterpret_cast<std::shared_ptr<function_profile_t> &>(profile_stack.at(pos))->decrement_cyclomatic_complexity_change();
            else
                reinterpret_cast<std::shared_ptr<function_profile_t> &>(profile_stack.at(pos))->increment_cyclomatic_complexity_change();

        }

    }

    //bool is_prototype = full_name == "parameter" || (full_name == "call" && profile_stack.at(profile_stack.size() - 2)->type_name == "member_init_list");
    if(is_statement(full_name)/* || is_prototype*/) {

        size_t parent_pos = counting_profile_pos.back();

        if(profile_stack.back()->operation == SRCDIFF_COMMON || profile_stack.back()->operation == SRCDIFF_DELETE)
            ++profile_stack.at(parent_pos)->statement_count_original;

        if(profile_stack.back()->operation == SRCDIFF_COMMON || profile_stack.back()->operation == SRCDIFF_INSERT)
            ++profile_stack.at(parent_pos)->statement_count_modified;

        ++profile_stack.at(parent_pos)->statement_count;

        if(profile_stack.back()->operation == SRCDIFF_COMMON)
            ++profile_stack.at(parent_pos)->common_statements;

        if(profile_stack.back()->operation != SRCDIFF_COMMON || profile_stack.back()->syntax_count)
            ++profile_stack.at(parent_pos)->statement_churn;

    }

    if(has_body(full_name)) {

        std::shared_ptr<unit_profile_t> & unit_profile = reinterpret_cast<std::shared_ptr<unit_profile_t> &>(profile_t::unit_profile);
        for(const std::pair<std::string, std::set<versioned_string>> & declaration : profile_stack.back()->declarations) {

            if(profile_stack.back()->id == unit_profile->identifier_to_declaration_profile[declaration.first].back()->id)
                unit_profile->identifier_to_declaration_profile[declaration.first].pop_back();

            if(unit_profile->identifier_to_declaration_profile[declaration.first].size() == 0)
                unit_profile->identifier_to_declaration_profile.erase(declaration.first);

        }

        profile_stack.at(counting_profile_pos.back())->statement_count_original += profile_stack.back()->statement_count_original;
        profile_stack.at(counting_profile_pos.back())->statement_count_modified += profile_stack.back()->statement_count_modified;
        profile_stack.at(counting_profile_pos.back())->statement_count += profile_stack.back()->statement_count;
        profile_stack.at(counting_profile_pos.back())->statement_churn += profile_stack.back()->statement_churn;
        profile_stack.at(counting_profile_pos.back())->common_statements += profile_stack.back()->common_statements;

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
    if(is_specifier(profile_stack.back()->type_name)) specifier_raw.append(ch,len);

    if(is_identifier(profile_stack.back()->type_name)
        || (profile_stack.back()->uri == SRCDIFF && is_identifier(profile_stack.back()->parent->type_name)))
        collected_simple_name.append(ch, len, srcdiff_stack.back().operation);
    if(name_count) collected_full_name.append(ch, len, srcdiff_stack.back().operation);
    if(condition_count) collected_condition.append(ch, len, srcdiff_stack.back().operation);

    for(std::vector<size_t>::size_type pos = 0; pos < expr_stmt_pos.size(); ++pos) {
 
        if(left_hand_side[pos])  collect_lhs[pos].append(ch, len, srcdiff_stack.back().operation);
        else                     collect_rhs[pos].append(ch, len, srcdiff_stack.back().operation);

    }

    for(std::pair<const size_t, std::string &> & raw_statement : raw_statements)
        raw_statement.second.append(ch, len);

}
