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

    typedef std::map<std::string, bool> class_profiles_t;
    typedef std::map<std::string, bool> function_profiles_t;

    enum srcdiff_type {

        SRCDIFF_COMMON,
        SRCDIFF_INSERT,
        SRCDIFF_DELETE,

    };

    struct srcdiff {

        srcdiff(srcdiff_type operation, bool is_change, bool is_move) : operation(operation), is_change(is_change), is_move(is_move), level(0) {}

        srcdiff_type operation;
        bool is_change;
        bool is_move;
        int level;

    };

    struct element {

        element(std::string name) : name(name), is_modified(false), is_whitespace(false), is_comment(false), is_syntax(false),  has_assignment(false) {}

        std::string name;
        bool is_modified;
        bool is_whitespace;
        bool is_comment;
        bool is_syntax;
        bool has_assignment;

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


            return out << "Whitespace: " << count.whitespace << " Comment: " << count. comment << " Syntax: " << count. syntax << " Total: " << count.total;

        }

        int whitespace;
        int comment;
        int syntax;
        int total;
    };

private:


protected:


    std::vector<srcdiff> srcdiff_stack;
    std::vector<element> element_stack;

    class_profiles_t & class_profiles;
    function_profiles_t & function_profiles;

    std::map<std::string, counts> inserted;
    std::map<std::string, counts> deleted;
    std::map<std::string, counts> modified;

    std::vector<int> counting_element_pos;

    counts insert_count;
    counts delete_count;
    counts change_count;
    counts total;

    std::string text;

private:

    bool is_simple_type(std::string & name) {

        return name == "throw" || name == "try" || name == "else" || name == "lit:literal" || name == "op:operator"
            || name == "type:modifier" || name == "cpp:if" || name == "cpp:elif" || name == "cpp:else"
            || name == "cpp:endif" || name == "cpp:ifdef" || name == "cpp:ifndef" || name == "cpp:pragma" || name == "cpp:error"
            || name == "cpp:region" || name == "cpp:endregion" || name == "case" || name == "default";

    }

    bool is_funct_type(std::string & name) {

        return name == "function" || name == "function_decl" || name == "constructor" || name == "constructor_decl"
            || name == "destructor" || name == "destructor_decl";

    }

    bool is_class_type(std::string & name) {

        return name == "class" || name == "class_decl" || name == "struct" || name == "struct_decl"
            || name == "union" || name == "union_decl" || name == "enum";

    }

    bool is_template(std::string & name) {

        return name == "template";

    }

    bool is_condition_type(std::string & name) {

        return name == "if" || name == "switch" || name == "elseif" || name == "while" || name == "do" || name == "for";

    }

    bool is_catch(std::string & name) {

        return name == "catch";

    }

    bool is_call(std::string & name) {

        return name == "call" || name == "macro";

    }

    bool is_decl_stmt(std::string & name) {

        return name == "decl_stmt";

    }

    bool is_preprocessor_special(std::string & name) {

        return name == "cpp:define" || name == "cpp:include" || name == "cpp:line" || name == "cpp:undef" || name == "cpp:ifdef" || name == "cpp:ifndef";

    }

    bool is_expr(std::string & name) {

    return name == "expr";

    }

    bool is_count(std::string & name) {

    return is_funct_type(name) || is_class_type(name) || is_simple_type(name)
        || is_condition_type(name) || is_catch(name) || is_decl_stmt(name)
        || is_call(name) || is_preprocessor_special(name) || is_expr(name)
        || is_template(name);

    }

    void update_diff(std::string name, bool is_whitespace) {

        if(srcdiff_stack.back().operation == SRCDIFF_INSERT) {

            if(inserted.find(name) == inserted.end())
                inserted[name] = counts();

            inserted[name].inc_total();

            if(is_whitespace)
                inserted[name].inc_whitespace();
            else
                inserted[name].inc_syntax();

        }

        else {

            if(deleted.find(name) == deleted.end())
                deleted[name] = counts();

            deleted[name].inc_total();

            if(is_whitespace)
                deleted[name].inc_whitespace();
            else
                deleted[name].inc_syntax();

        }

    }

    void count_diff(bool is_whitespace) {

        if(counting_element_pos.empty())
            return;

        for(int i = 0; i < counting_element_pos.size(); ++i) {

            int pos = counting_element_pos.at(i);
            std::string name = element_stack.at(pos).name;

        if(is_expr(name)) continue;

            if(pos == (element_stack.size() - 1)) update_diff(name, is_whitespace);

            else if(pos != (element_stack.size() - 1)) {

                if(is_funct_type(name)) {

                    if((pos + 1) == (element_stack.size() - 1) && (element_stack.back().name == "template"))
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);
                    else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "parameter_list" && element_stack.back().name == "param")
                        update_diff(name + "/param", is_whitespace);
                    else if((pos + 2) < (element_stack.size()) && element_stack.at(pos + 1).name == "parameter_list" && element_stack.at(pos + 2).name == "param"
                            && element_stack.back().name == "init")
                        update_diff(name + "/param/init", is_whitespace);
                    else if(element_stack.back().name == "return")
                        update_diff(name + "/return", is_whitespace);
                    else if((pos + 2) == (element_stack.size() - 1) && (element_stack.back().name == "member_list"))
                        update_diff(name + "/member_list", is_whitespace);
                    else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "member_list"
                && (pos + 3) == (element_stack.size() - 1) && element_stack.back().name == "call")
                        update_diff(name + "/member_list/call", is_whitespace);

                } else if(is_class_type(name)) {

                    if((pos + 2) == (element_stack.size() - 1)
                       && (element_stack.back().name == "decl"
                           || element_stack.back().name == "super"
                           || element_stack.back().name == "template"))
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);
                    else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "block"
                            && (pos + 3) == (element_stack.size() - 1) 
                && (element_stack.back().name == "private" || element_stack.back().name == "public"
                                || element_stack.back().name == "protected" || element_stack.back().name == "signals"))
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);
                    else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "block"
                            && ((pos + 3) == (element_stack.size() - 1) 
                            || ((pos + 4) == (element_stack.size() - 1) 
                            && (element_stack.at(pos + 2).name == "private" || element_stack.at(pos + 2).name == "public"
                            || element_stack.at(pos + 2).name == "protected" || element_stack.at(pos + 2).name == "signals")))
                            && (element_stack.back().name == "decl_stmt" || element_stack.back().name == "template"
                            || is_funct_type(element_stack.back().name) || is_class_type(element_stack.back().name))) {

                                if(element_stack.back().name == "template")
                                    update_diff(name + "/block/" + element_stack.back().name, is_whitespace);
                                else
                                    update_diff(name + "/" + element_stack.back().name, is_whitespace);
                        
                    } else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "block" && element_stack.at(element_stack.size() - 2).name == "template") {

                        int count_template = 1;
                        while(((count_template + 2) < element_stack.size()) && element_stack.at((element_stack.size() - count_template) - 2).name == "template")
                            ++count_template;

                        if(((pos + count_template + 3) == (element_stack.size() - 1) 
                            || ((pos + count_template + 4) == (element_stack.size() - 1) 
                            && (element_stack.at(pos + 2).name == "private" || element_stack.at(pos + 2).name == "public"
                            || element_stack.at(pos + 2).name == "protected" || element_stack.at(pos + 2).name == "signals")))
                            && (is_funct_type(element_stack.back().name) || is_class_type(element_stack.back().name)
                                 || is_decl_stmt(element_stack.back().name))) {

                                update_diff(name + "/" + element_stack.back().name, is_whitespace);
                                update_diff(name + "/block/template/" + element_stack.back().name, is_whitespace);

                        }

                    }

                } else if(is_template(name)) {

                    if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "parameter_list" && element_stack.back().name == "param")
                        update_diff(name + "/param", is_whitespace);
                    else if((pos + 1) == (element_stack.size() - 1) && (is_funct_type(element_stack.back().name) || is_class_type(element_stack.back().name)
                         || is_decl_stmt(element_stack.back().name)))
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);

                } else if(is_condition_type(name)) {

                if((pos + 2) == (element_stack.size() - 1)
                        && (element_stack.back().name == "incr"
                            || element_stack.back().name == "case"
                            || element_stack.back().name == "default"))
                    update_diff(name + "/" + element_stack.back().name, is_whitespace);
                else if((pos + 3) == (element_stack.size() - 1)
                        && (element_stack.back().name == "case"
                            || element_stack.back().name == "default"))
                    update_diff(name + "/" + element_stack.back().name, is_whitespace);

                } else if(is_catch(name)) {

                    if((pos + 2) == (element_stack.size() - 1)
                       && element_stack.back().name == "param")
                        update_diff(name + "/param", is_whitespace);
                    else if((pos + 1) < (element_stack.size())
                       && element_stack.at(pos + 1).name == "parameter_list"
                       && (pos + 3) == (element_stack.size() - 1)
                       && element_stack.back().name == "param")
                        update_diff(name + "/param", is_whitespace);


                } else if(is_decl_stmt(name)) {

                    if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "decl" && (pos + 3) == (element_stack.size() - 1)
                       && (element_stack.back().name == "init" || element_stack.back().name == "name" || element_stack.back().name == "template"))
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);
                    else if(element_stack.back().name == "decl")
                        update_diff(name + "/decl", is_whitespace);

                } else if(is_call(name)) {

                    if((pos + 3) == (element_stack.size() - 1)
                       && element_stack.back().name == "argument")
                        update_diff(name + "/argument", is_whitespace);

                } else if(is_preprocessor_special(name)) {

                    if((pos + 2) == (element_stack.size() - 1)
                       && (element_stack.back().name == "name"
                           || element_stack.back().name == "cpp:macro"
                           || element_stack.back().name == "cpp:value"
                           || element_stack.back().name == "cpp:number"
                           || element_stack.back().name == "cpp:file"))
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);
                    else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "cpp:macro"
                       && (pos + 3) == (element_stack.size() - 1)
                       && element_stack.back().name == "parameter_list")
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);
                    else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "cpp:macro" 
                && element_stack.at(pos + 2).name == "parameter_list"
                && (pos + 4) == (element_stack.size() - 1)
                && element_stack.back().name == "param")
                        update_diff(name + "/" + element_stack.back().name, is_whitespace);

                }

            }

        }

    }

    void update_modified(std::string name) {

        if(modified.find(name) == modified.end())
            modified[name] = counts();

        modified[name].inc_total();

        if(element_stack.back().is_whitespace)
            modified[name].inc_whitespace();

        if(element_stack.back().is_comment)
            modified[name].inc_comment();

        if(element_stack.back().is_syntax)
            modified[name].inc_syntax();

    }

    void count_modified() {

        if(counting_element_pos.empty() || srcdiff_stack.back().operation != SRCDIFF_COMMON)
            return;

        for(int i = 0; i < counting_element_pos.size(); ++i) {

            int pos = counting_element_pos.at(i);
            std::string name = element_stack.at(pos).name;

            if(pos == (element_stack.size() - 1)) {

            if(!is_expr(name)) update_modified(name);
            else if(element_stack.back().has_assignment) update_modified(name + "/assignment");

            } else if(is_funct_type(name)) {

                if((pos + 1) == (element_stack.size() - 1)
                   && (element_stack.at(pos + 1).name == "type"
                       || element_stack.at(pos + 1).name == "name"
                       || element_stack.at(pos + 1).name == "parameter_list"
                       || element_stack.at(pos + 1).name == "block"
                       || element_stack.at(pos + 1).name == "member_list"
                       || element_stack.at(pos + 1).name == "template"))
                    update_modified(name + "/" + element_stack.at(pos + 1).name);
                else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "parameter_list"
                        && (pos + 2) == (element_stack.size() - 1)
                          && element_stack.back().name == "param")
                    update_modified(name + "/param");
                else if((pos + 2) < (element_stack.size()) && element_stack.at(pos + 1).name == "parameter_list"
                        && element_stack.at(pos + 2).name == "param"
                        && element_stack.back().name == "init")
                    update_modified(name + "/param/init");
                else if(element_stack.back().name == "return")
                    update_modified(name + "/return");
                else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "member_list"
                        && (pos + 2) == (element_stack.size() - 1)
                && element_stack.back().name == "call")
                    update_modified(name + "/member_list/call");


            } else if(is_class_type(name)) {

                if((element_stack.size() - 1) == (pos + 1)
                   && (element_stack.back().name == "super"
                       || element_stack.back().name == "name"
                       || element_stack.back().name == "block"
                       || element_stack.back().name == "decl"
                       || element_stack.back().name == "template"))
                    update_modified(name + "/" + element_stack.back().name);
            else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "block"
                && (pos + 2) == (element_stack.size() - 1)
                && (element_stack.back().name == "private" || element_stack.back().name == "public"
                || element_stack.back().name == "protected" || element_stack.back().name == "signals"))
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "block"
                        && ((pos + 2) == (element_stack.size() - 1) 
                || ((pos + 3) == (element_stack.size() - 1) 
                    && (element_stack.at(pos + 2).name == "private" || element_stack.at(pos + 2).name == "public"
                    || element_stack.at(pos + 2).name == "protected" || element_stack.at(pos + 2).name == "signals")))
                        && (element_stack.back().name == "decl_stmt" || element_stack.back().name == "template"
                            || is_funct_type(element_stack.back().name) || is_class_type(element_stack.back().name))) {

                            if(element_stack.back().name == "template")
                                update_modified(name + "/block/" + element_stack.back().name);
                            else
                                update_modified(name + "/" + element_stack.back().name);

                } else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "block" && element_stack.at(element_stack.size() - 2).name == "template")  {

                        int count_template = 1;
                        while(((count_template + 2) < element_stack.size()) && element_stack.at((element_stack.size() - count_template) - 2).name == "template")
                            ++count_template;

                        if(((pos + count_template + 2) == (element_stack.size() - 1) 
                            || ((pos + count_template + 3) == (element_stack.size() - 1) 
                            && (element_stack.at(pos + 2).name == "private" || element_stack.at(pos + 2).name == "public"
                            || element_stack.at(pos + 2).name == "protected" || element_stack.at(pos + 2).name == "signals")))
                            && (is_funct_type(element_stack.back().name) || is_class_type(element_stack.back().name)
                                || is_decl_stmt(element_stack.back().name))) {

                                update_modified(name + "/" + element_stack.back().name);
                                update_modified(name + "/block/template/" + element_stack.back().name);

                        }

                    }

            } else if(is_template(name)) { 

                if((pos + 1) == (element_stack.size() - 1) && element_stack.back().name == "parameter_list")
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 1) == (element_stack.size() - 1) && (is_funct_type(element_stack.back().name) || is_class_type(element_stack.back().name)
                    || is_decl_stmt(element_stack.back().name)))
                        update_modified(name + "/" + element_stack.back().name);
                else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "parameter_list"
                          && element_stack.back().name == "param")
                    update_modified(name + "/param");

            } else if(is_condition_type(name)) {

                if((pos + 1) == (element_stack.size() - 1)
                   && (element_stack.back().name == "init"
                       || element_stack.back().name == "condition"
                       || element_stack.back().name == "incr"
                       || element_stack.back().name == "then"
                       || element_stack.back().name == "case"
                       || element_stack.back().name == "default"))
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 2) == (element_stack.size() - 1)
                       && (element_stack.back().name == "case"
                       || element_stack.back().name == "default"))
                    update_modified(name + "/" + element_stack.back().name);

            } else if(is_catch(name)) {

                if((pos + 1) == (element_stack.size() - 1)
                   && (element_stack.back().name == "parameter_list"
                       || element_stack.back().name == "param"
                       || element_stack.back().name == "block"))
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 2) < (element_stack.size())
                       && element_stack.at(pos + 1).name == "parameter_list"
                       && element_stack.back().name == "param")
                    update_modified(name + "/param");

            } else if(is_decl_stmt(name)) {

                if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "decl"
                   && (pos + 2) == (element_stack.size() - 1)
                   && (element_stack.back().name == "type"
                       || element_stack.back().name == "name"
                       || element_stack.back().name == "init"
                       || element_stack.back().name == "template"))
                    update_modified(name + "/" + element_stack.back().name);

            } else if(is_call(name)) {

                if((pos + 1) == (element_stack.size() - 1)
                   && (element_stack.at(pos + 1).name == "name"
                       || element_stack.back().name == "argument_list"))
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "argument_list"
                   && (pos + 2) == (element_stack.size() - 1)
                   && element_stack.back().name == "argument")
                    update_modified(name + "/argument");

            } else if(is_preprocessor_special(name)) {

            if((pos + 1) == (element_stack.size() - 1)
               && (element_stack.back().name == "name"
                       || element_stack.back().name == "cpp:macro"
                       || element_stack.back().name == "cpp:value"
                       || element_stack.back().name == "cpp:number"
                       || element_stack.back().name == "cpp:file"))
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 1) < (element_stack.size()) && element_stack.at(pos + 1).name == "cpp:macro"
                   && (pos + 2) == (element_stack.size() - 1)
                   && (element_stack.back().name == "parameter_list"
               || element_stack.back().name == "name"))
                    update_modified(name + "/" + element_stack.back().name);
                else if((pos + 2) < (element_stack.size()) && element_stack.at(pos + 1).name == "cpp:macro"
               && element_stack.at(pos + 2).name == "parameter_list"
                   && (pos + 3) == (element_stack.size() - 1)
                   && element_stack.back().name == "param")
                    update_modified(name + "/param");

            }

        }

    }

    void process_characters() {

       if(element_stack.back().name == "op:operator") {

        if(text == "=" || text == "+=" || text == "-=" || text == "*=" || text == "/=" || text == ">>=" || text == "<<=" || text == "%=" || text == "&=" || text == "|=" || text == "^=")
            element_stack.back().has_assignment = true;
        else
            element_stack.back().has_assignment = false;

       }

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

                if(element_stack.back().name == "comment") {

                    element_stack.at(element_stack.size() - 2).is_comment = true;
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

                    element_stack.at(element_stack.size() - 2).is_whitespace = true;
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

                    element_stack.at(element_stack.size() - 2).is_syntax = true;
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

                element_stack.push_back(element("text"));

                count_diff(is_whitespace);

                element_stack.pop_back();

            }


        }

        text = "";

    }

public:

    srcdiff_summary_handler(class_profiles_t & class_profiles, function_profiles_t & function_profiles) : 
        srcdiff_stack(), element_stack(), class_profiles(class_profiles), function_profiles(function_profiles), 
        inserted(), deleted(), modified(), insert_count(), delete_count(), change_count(), total(), text() {}

    /**
     * startDocument
     *
     * SAX handler function for start of document.
     * Overide for desired behaviour.
     */
    virtual void startDocument() {

        srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, false, false));

    }

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
                           const struct srcsax_attribute * attributes) {

        if(text != "") process_characters();

    }

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
                           const struct srcsax_attribute * attributes) {

        if(text != "") process_characters();

        std::string full_name = "";

        if(prefix) {

            full_name += (const char *)prefix;
            full_name += ":";

        }

        full_name += (const char *)localname;

        element_stack.push_back(full_name);

    }

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

        if(text != "") process_characters();

        if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

            /* @todo check if move and put as in common */
            bool is_change = false;
            bool is_move = false;
            for(int i = 0; i < num_attributes; ++i) {

                if(strcmp((const char *)attributes[i].localname, "type") == 0) {

                    std::string value = attributes[i].value;
                    if(value == "change") is_change = true;

                } else if(strcmp((const char *)attributes[i].localname, "move") == 0) {

                    is_move = true;

                }

            }

            if(strcmp((const char *)localname, "common") == 0 || is_move)
                srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, is_change, is_move));
            else if(strcmp((const char *)localname, "delete") == 0)
                srcdiff_stack.push_back(srcdiff(SRCDIFF_DELETE, is_change, is_move));
            else if(strcmp((const char *)localname, "insert") == 0)
                srcdiff_stack.push_back(srcdiff(SRCDIFF_INSERT, is_change, is_move));

            if((srcdiff_stack.back().operation == SRCDIFF_DELETE || srcdiff_stack.back().operation == SRCDIFF_INSERT)
               && srcdiff_stack.at(srcdiff_stack.size() - 2).operation == SRCDIFF_COMMON)
                element_stack.back().is_modified = true;

        }

        std::string full_name = "";

        if(prefix) {

            full_name += (const char *)prefix;
            full_name += ":";

        }

        full_name += (const char *)localname;

        element_stack.push_back(element(full_name));

        if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0) {

            ++srcdiff_stack.back().level;

            if(is_count(full_name))
                counting_element_pos.push_back(element_stack.size() - 1);

            if(srcdiff_stack.back().operation != SRCDIFF_COMMON) {

                if(srcdiff_stack.back().level == 1) {

                    if(strcmp((const char *)localname, "comment") == 0) {

                        total.inc_comment();
                        element_stack.at(element_stack.size() - 3).is_comment = true;

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
                        element_stack.at(element_stack.size() - 3).is_syntax = true;

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

                    count_diff(false);

                } else if(srcdiff_stack.back().level == 2 && element_stack.at(element_stack.size() - 2).name == "template"
                     && (is_funct_type(full_name) || is_class_type(full_name) || is_decl_stmt(full_name))) {

                    count_diff(false);

                }

            }

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
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {

        if(text != "") process_characters();        

    }

    /**
     * endUnit
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     *
     * SAX handler function for end of an unit.
     * Overide for desired behaviour.
     */
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {

        if(text != "") process_characters();

        element_stack.pop_back();

    }

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


        if(text != "") process_characters();

        if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") == 0) {

            if(strcmp((const char *)localname, "common") == 0
               || strcmp((const char *)localname, "delete") == 0
               || strcmp((const char *)localname, "insert") == 0)
                srcdiff_stack.pop_back();

        }

        std::string full_name = "";

        if(prefix) {

            full_name += (const char *)prefix;
            full_name += ":";

        }

        full_name += (const char *)localname;

        if(strcmp((const char *)URI, "http://www.sdml.info/srcDiff") != 0) {

            --srcdiff_stack.back().level;

        if(element_stack.back().has_assignment
           && element_stack.at(element_stack.size() - 2).name != "diff:delete"
           && element_stack.at(element_stack.size() - 2).name != "diff:insert"
           && element_stack.at(element_stack.size() - 2).name != "diff:common")
        element_stack.at(element_stack.size() - 2).has_assignment = true;


            if(element_stack.back().is_modified) {

                count_modified();

                if(element_stack.at(element_stack.size() - 2).name != "diff:delete"
                   && element_stack.at(element_stack.size() - 2).name != "diff:insert"
                   && element_stack.at(element_stack.size() - 2).name != "diff:common") {

                    element_stack.at(element_stack.size() - 2).is_modified = true;

                    if(element_stack.back().is_whitespace)
                        element_stack.at(element_stack.size() - 2).is_whitespace = true;


                    if(element_stack.back().is_comment)
                        element_stack.at(element_stack.size() - 2).is_comment = true;

                    if(element_stack.back().is_syntax)
                        element_stack.at(element_stack.size() - 2).is_syntax = true;

                }

            }

            if(is_count(full_name))
                counting_element_pos.pop_back();

        }

        element_stack.pop_back();

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


        if(len == 0) return;

        text.append((const char *)ch, len);

    }

};

#endif
