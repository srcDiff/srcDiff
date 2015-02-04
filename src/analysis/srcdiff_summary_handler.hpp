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

    bool is_simple_type(const std::string & name) {

        return name == "throw" || name == "try" || name == "else" || name == "lit:literal" || name == "op:operator"
            || name == "type:modifier" || name == "cpp:if" || name == "cpp:elif" || name == "cpp:else"
            || name == "cpp:endif" || name == "cpp:ifdef" || name == "cpp:ifndef" || name == "cpp:pragma" || name == "cpp:error"
            || name == "cpp:region" || name == "cpp:endregion" || name == "case" || name == "default";

    }

    bool is_funct_type(const std::string & name) {

        return name == "function" || name == "function_decl" || name == "constructor" || name == "constructor_decl"
            || name == "destructor" || name == "destructor_decl";

    }

    bool is_class_type(const std::string & name) {

        return name == "class" || name == "class_decl" || name == "struct" || name == "struct_decl"
            || name == "union" || name == "union_decl" || name == "enum";

    }

    bool is_template(const std::string & name) {

        return name == "template";

    }

    bool is_condition_type(const std::string & name) {

        return name == "if" || name == "switch" || name == "elseif" || name == "while" || name == "do" || name == "for";

    }

    bool is_catch(const std::string & name) {

        return name == "catch";

    }

    bool is_call(const std::string & name) {

        return name == "call" || name == "macro";

    }

    bool is_decl_stmt(const std::string & name) {

        return name == "decl_stmt";

    }

    bool is_preprocessor_special(const std::string & name) {

        return name == "cpp:define" || name == "cpp:include" || name == "cpp:line" || name == "cpp:undef" || name == "cpp:ifdef" || name == "cpp:ifndef";

    }

    bool is_expr(const std::string & name) {

    return name == "expr";

    }

    bool is_count(const std::string & name) {

    return is_funct_type(name) || is_class_type(name) || is_simple_type(name)
        || is_condition_type(name) || is_catch(name) || is_decl_stmt(name)
        || is_call(name) || is_preprocessor_special(name) || is_expr(name)
        || is_template(name);

    }

    void update_diff_map(std::map<std::string, counts> & map, const std::string & name, bool is_whitespace) {

        if(map.find(name) == map.end())
            map[name] = counts();

        map[name].inc_total();

        if(is_whitespace)
            map[name].inc_whitespace();
        else
            map[name].inc_syntax();


    }

    void update_diff(const std::string & name, size_t profile_pos, bool is_whitespace) {


        if(srcdiff_stack.back().operation == SRCDIFF_INSERT) {

            // global lists
            update_diff_map(inserted, name, is_whitespace);

            // profile lists
            update_diff_map(profile_stack.at(profile_pos).inserted, name, is_whitespace);


        }

        else {

            // global lists
            update_diff_map(deleted, name, is_whitespace);

            // profile lists
            update_diff_map(profile_stack.at(profile_pos).deleted, name, is_whitespace);

        }

    }

    void count_diff(bool is_whitespace) {

        if(counting_profile_pos.empty())
            return;

        for(size_t i = 0; i < counting_profile_pos.size(); ++i) {

            size_t pos = counting_profile_pos.at(i);
            std::string name = profile_stack.at(pos).type_name;

        if(is_expr(name)) continue;

            if(pos == (profile_stack.size() - 1)) update_diff(name, pos, is_whitespace);

            else if(pos != (profile_stack.size() - 1)) {

                if(is_funct_type(name)) {

                    if((pos + 1) == (profile_stack.size() - 1) && (profile_stack.back().type_name == "template"))
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "parameter_list" && profile_stack.back().type_name == "parameter")
                        update_diff(name + "/parameter", pos, is_whitespace);
                    else if((pos + 2) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "parameter_list" && profile_stack.at(pos + 2).type_name == "parameter"
                            && profile_stack.back().type_name == "init")
                        update_diff(name + "/param/init", pos, is_whitespace);
                    else if(profile_stack.back().type_name == "return")
                        update_diff(name + "/return", pos, is_whitespace);
                    else if((pos + 2) == (profile_stack.size() - 1) && (profile_stack.back().type_name == "member_list"))
                        update_diff(name + "/member_list", pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "member_list"
                && (pos + 3) == (profile_stack.size() - 1) && profile_stack.back().type_name == "call")
                        update_diff(name + "/member_list/call", pos, is_whitespace);

                } else if(is_class_type(name)) {

                    if((pos + 2) == (profile_stack.size() - 1)
                       && (profile_stack.back().type_name == "decl"
                           || profile_stack.back().type_name == "super"
                           || profile_stack.back().type_name == "template"))
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "block"
                            && (pos + 3) == (profile_stack.size() - 1) 
                && (profile_stack.back().type_name == "private" || profile_stack.back().type_name == "public"
                                || profile_stack.back().type_name == "protected" || profile_stack.back().type_name == "signals"))
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "block"
                            && ((pos + 3) == (profile_stack.size() - 1) 
                            || ((pos + 4) == (profile_stack.size() - 1) 
                            && (profile_stack.at(pos + 2).type_name == "private" || profile_stack.at(pos + 2).type_name == "public"
                            || profile_stack.at(pos + 2).type_name == "protected" || profile_stack.at(pos + 2).type_name == "signals")))
                            && (profile_stack.back().type_name == "decl_stmt" || profile_stack.back().type_name == "template"
                            || is_funct_type(profile_stack.back().type_name) || is_class_type(profile_stack.back().type_name))) {

                                if(profile_stack.back().type_name == "template")
                                    update_diff(name + "/block/" + profile_stack.back().type_name, pos, is_whitespace);
                                else
                                    update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                        
                    } else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "block" && profile_stack.at(profile_stack.size() - 2).type_name == "template") {

                        int count_template = 1;
                        while(((count_template + 2) < profile_stack.size()) && profile_stack.at((profile_stack.size() - count_template) - 2).type_name == "template")
                            ++count_template;

                        if(((pos + count_template + 3) == (profile_stack.size() - 1) 
                            || ((pos + count_template + 4) == (profile_stack.size() - 1) 
                            && (profile_stack.at(pos + 2).type_name == "private" || profile_stack.at(pos + 2).type_name == "public"
                            || profile_stack.at(pos + 2).type_name == "protected" || profile_stack.at(pos + 2).type_name == "signals")))
                            && (is_funct_type(profile_stack.back().type_name) || is_class_type(profile_stack.back().type_name)
                                 || is_decl_stmt(profile_stack.back().type_name))) {

                                update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                                update_diff(name + "/block/template/" + profile_stack.back().type_name, pos, is_whitespace);

                        }

                    }

                } else if(is_template(name)) {

                    if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "parameter_list" && profile_stack.back().type_name == "parameter")
                        update_diff(name + "/parameter", pos, is_whitespace);
                    else if((pos + 1) == (profile_stack.size() - 1) && (is_funct_type(profile_stack.back().type_name) || is_class_type(profile_stack.back().type_name)
                         || is_decl_stmt(profile_stack.back().type_name)))
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);

                } else if(is_condition_type(name)) {

                if((pos + 2) == (profile_stack.size() - 1)
                        && (profile_stack.back().type_name == "incr"
                            || profile_stack.back().type_name == "case"
                            || profile_stack.back().type_name == "default"))
                    update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                else if((pos + 3) == (profile_stack.size() - 1)
                        && (profile_stack.back().type_name == "case"
                            || profile_stack.back().type_name == "default"))
                    update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);

                } else if(is_catch(name)) {

                    if((pos + 2) == (profile_stack.size() - 1)
                       && profile_stack.back().type_name == "parameter")
                        update_diff(name + "/parameter", pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size())
                       && profile_stack.at(pos + 1).type_name == "parameter_list"
                       && (pos + 3) == (profile_stack.size() - 1)
                       && profile_stack.back().type_name == "parameter")
                        update_diff(name + "/parameter", pos, is_whitespace);


                } else if(is_decl_stmt(name)) {

                    if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "decl" && (pos + 3) == (profile_stack.size() - 1)
                       && (profile_stack.back().type_name == "init" || profile_stack.back().type_name == "name" || profile_stack.back().type_name == "template"))
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                    else if(profile_stack.back().type_name == "decl")
                        update_diff(name + "/decl", pos, is_whitespace);

                } else if(is_call(name)) {

                    if((pos + 3) == (profile_stack.size() - 1)
                       && profile_stack.back().type_name == "argument")
                        update_diff(name + "/argument", pos, is_whitespace);

                } else if(is_preprocessor_special(name)) {

                    if((pos + 2) == (profile_stack.size() - 1)
                       && (profile_stack.back().type_name == "name"
                           || profile_stack.back().type_name == "cpp:macro"
                           || profile_stack.back().type_name == "cpp:value"
                           || profile_stack.back().type_name == "cpp:number"
                           || profile_stack.back().type_name == "cpp:file"))
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "cpp:macro"
                       && (pos + 3) == (profile_stack.size() - 1)
                       && profile_stack.back().type_name == "parameter_list")
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);
                    else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "cpp:macro" 
                && profile_stack.at(pos + 2).type_name == "parameter_list"
                && (pos + 4) == (profile_stack.size() - 1)
                && profile_stack.back().type_name == "parameter")
                        update_diff(name + "/" + profile_stack.back().type_name, pos, is_whitespace);

                }

            }

        }

    }

    void update_modified(const std::string & name, size_t profile_pos) {

        if(modified.find(name) == modified.end())
            modified[name] = counts();

        if(profile_stack.at(profile_pos).modified.find(name) == profile_stack.at(profile_pos).modified.end())
            profile_stack.at(profile_pos).modified[name] = counts();

        modified[name].inc_total();

        if(profile_stack.back().is_whitespace) {

            modified[name].inc_whitespace();
            profile_stack.at(profile_pos).modified[name].inc_whitespace();

        }

        if(profile_stack.back().is_comment) {

            modified[name].inc_comment();
            profile_stack.at(profile_pos).modified[name].inc_comment();

        }

        if(profile_stack.back().is_syntax) {

            modified[name].inc_syntax();
            profile_stack.at(profile_pos).modified[name].inc_syntax();

        }

    }

    void count_modified() {

        if(counting_profile_pos.empty() || srcdiff_stack.back().operation != SRCDIFF_COMMON)
            return;

        for(int i = 0; i < counting_profile_pos.size(); ++i) {

            int pos = counting_profile_pos.at(i);
            std::string name = profile_stack.at(pos).type_name;

            if(pos == (profile_stack.size() - 1)) {

            if(!is_expr(name)) update_modified(name, pos);
            else if(profile_stack.back().has_assignment) update_modified(name + "/assignment", pos);

            } else if(is_funct_type(name)) {

                if((pos + 1) == (profile_stack.size() - 1)
                   && (profile_stack.at(pos + 1).type_name == "type"
                       || profile_stack.at(pos + 1).type_name == "name"
                       || profile_stack.at(pos + 1).type_name == "parameter_list"
                       || profile_stack.at(pos + 1).type_name == "block"
                       || profile_stack.at(pos + 1).type_name == "member_list"
                       || profile_stack.at(pos + 1).type_name == "template"))
                    update_modified(name + "/" + profile_stack.at(pos + 1).type_name, pos);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "parameter_list"
                        && (pos + 2) == (profile_stack.size() - 1)
                          && profile_stack.back().type_name == "parameter")
                    update_modified(name + "/parameter", pos);
                else if((pos + 2) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "parameter_list"
                        && profile_stack.at(pos + 2).type_name == "parameter"
                        && profile_stack.back().type_name == "init")
                    update_modified(name + "/param/init", pos);
                else if(profile_stack.back().type_name == "return")
                    update_modified(name + "/return", pos);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "member_list"
                        && (pos + 2) == (profile_stack.size() - 1)
                && profile_stack.back().type_name == "call")
                    update_modified(name + "/member_list/call", pos);

            } else if(is_class_type(name)) {

                if((profile_stack.size() - 1) == (pos + 1)
                   && (profile_stack.back().type_name == "super"
                       || profile_stack.back().type_name == "name"
                       || profile_stack.back().type_name == "block"
                       || profile_stack.back().type_name == "decl"
                       || profile_stack.back().type_name == "template"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "block"
                && (pos + 2) == (profile_stack.size() - 1)
                && (profile_stack.back().type_name == "private" || profile_stack.back().type_name == "public"
                || profile_stack.back().type_name == "protected" || profile_stack.back().type_name == "signals"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "block"
                        && ((pos + 2) == (profile_stack.size() - 1) 
                || ((pos + 3) == (profile_stack.size() - 1) 
                    && (profile_stack.at(pos + 2).type_name == "private" || profile_stack.at(pos + 2).type_name == "public"
                    || profile_stack.at(pos + 2).type_name == "protected" || profile_stack.at(pos + 2).type_name == "signals")))
                        && (profile_stack.back().type_name == "decl_stmt" || profile_stack.back().type_name == "template"
                            || is_funct_type(profile_stack.back().type_name) || is_class_type(profile_stack.back().type_name))) {

                            if(profile_stack.back().type_name == "template")
                                update_modified(name + "/block/" + profile_stack.back().type_name, pos);
                            else
                                update_modified(name + "/" + profile_stack.back().type_name, pos);

                } else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "block" && profile_stack.at(profile_stack.size() - 2).type_name == "template")  {

                        int count_template = 1;
                        while(((count_template + 2) < profile_stack.size()) && profile_stack.at((profile_stack.size() - count_template) - 2).type_name == "template")
                            ++count_template;

                        if(((pos + count_template + 2) == (profile_stack.size() - 1) 
                            || ((pos + count_template + 3) == (profile_stack.size() - 1) 
                            && (profile_stack.at(pos + 2).type_name == "private" || profile_stack.at(pos + 2).type_name == "public"
                            || profile_stack.at(pos + 2).type_name == "protected" || profile_stack.at(pos + 2).type_name == "signals")))
                            && (is_funct_type(profile_stack.back().type_name) || is_class_type(profile_stack.back().type_name)
                                || is_decl_stmt(profile_stack.back().type_name))) {

                                update_modified(name + "/" + profile_stack.back().type_name, pos);
                                update_modified(name + "/block/template/" + profile_stack.back().type_name, pos);

                        }

                    }

            } else if(is_template(name)) { 

                if((pos + 1) == (profile_stack.size() - 1) && profile_stack.back().type_name == "parameter_list")
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 1) == (profile_stack.size() - 1) && (is_funct_type(profile_stack.back().type_name) || is_class_type(profile_stack.back().type_name)
                    || is_decl_stmt(profile_stack.back().type_name)))
                        update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "parameter_list"
                          && profile_stack.back().type_name == "parameter")
                    update_modified(name + "/parameter", pos);

            } else if(is_condition_type(name)) {

                if((pos + 1) == (profile_stack.size() - 1)
                   && (profile_stack.back().type_name == "init"
                       || profile_stack.back().type_name == "condition"
                       || profile_stack.back().type_name == "incr"
                       || profile_stack.back().type_name == "then"
                       || profile_stack.back().type_name == "case"
                       || profile_stack.back().type_name == "default"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 2) == (profile_stack.size() - 1)
                       && (profile_stack.back().type_name == "case"
                       || profile_stack.back().type_name == "default"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);

            } else if(is_catch(name)) {

                if((pos + 1) == (profile_stack.size() - 1)
                   && (profile_stack.back().type_name == "parameter_list"
                       || profile_stack.back().type_name == "parameter"
                       || profile_stack.back().type_name == "block"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 2) < (profile_stack.size())
                       && profile_stack.at(pos + 1).type_name == "parameter_list"
                       && profile_stack.back().type_name == "parameter")
                    update_modified(name + "/parameter", pos);

            } else if(is_decl_stmt(name)) {

                if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "decl"
                   && (pos + 2) == (profile_stack.size() - 1)
                   && (profile_stack.back().type_name == "type"
                       || profile_stack.back().type_name == "name"
                       || profile_stack.back().type_name == "init"
                       || profile_stack.back().type_name == "template"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);

            } else if(is_call(name)) {

                if((pos + 1) == (profile_stack.size() - 1)
                   && (profile_stack.at(pos + 1).type_name == "name"
                       || profile_stack.back().type_name == "argument_list"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "argument_list"
                   && (pos + 2) == (profile_stack.size() - 1)
                   && profile_stack.back().type_name == "argument")
                    update_modified(name + "/argument", pos);

            } else if(is_preprocessor_special(name)) {

            if((pos + 1) == (profile_stack.size() - 1)
               && (profile_stack.back().type_name == "name"
                       || profile_stack.back().type_name == "cpp:macro"
                       || profile_stack.back().type_name == "cpp:value"
                       || profile_stack.back().type_name == "cpp:number"
                       || profile_stack.back().type_name == "cpp:file"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "cpp:macro"
                   && (pos + 2) == (profile_stack.size() - 1)
                   && (profile_stack.back().type_name == "parameter_list"
               || profile_stack.back().type_name == "name"))
                    update_modified(name + "/" + profile_stack.back().type_name, pos);
                else if((pos + 2) < (profile_stack.size()) && profile_stack.at(pos + 1).type_name == "cpp:macro"
               && profile_stack.at(pos + 2).type_name == "parameter_list"
                   && (pos + 3) == (profile_stack.size() - 1)
                   && profile_stack.back().type_name == "parameter")
                    update_modified(name + "/parameter", pos);

            }

        }

    }

    void process_characters() {

       if(profile_stack.back().type_name == "op:operator") {

        if(text == "=" || text == "+=" || text == "-=" || text == "*=" || text == "/=" || text == ">>=" || text == "<<=" || text == "%=" || text == "&=" || text == "|=" || text == "^=") {

            profile_stack.back().has_assignment = true;
            ++profile_stack.back().assignment_count;

        } else
            profile_stack.back().has_assignment = false;
            profile_stack.back().assignment_count = 0;

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

                if(profile_stack.back().type_name == "comment") {

                    profile_stack.at(profile_stack.size() - 2).is_comment = true;
                    ++profile_stack.at(profile_stack.size() - 2).comment_count;
                    ++profile_stack.at(profile_stack.size() - 2).total_count;
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

                    profile_stack.at(profile_stack.size() - 2).is_whitespace = true;
                    ++profile_stack.at(profile_stack.size() - 2).whitespace_count;
                    ++profile_stack.at(profile_stack.size() - 2).total_count;
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

                    profile_stack.at(profile_stack.size() - 2).is_syntax = true;
                    ++profile_stack.at(profile_stack.size() - 2).syntax_count;
                    ++profile_stack.at(profile_stack.size() - 2).total_count;
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

                profile_stack.push_back(profile_t("text"));

                count_diff(is_whitespace);

                profile_stack.pop_back();

            }


        }

        text = "";

    }

public:

    srcdiff_summary_handler(profile_list_t & profile_list) : profile_list(profile_list), srcdiff_stack(), profile_stack(), counting_profile_pos(),
        inserted(), deleted(), modified(), insert_count(), delete_count(), change_count(), total(), text(),
        name_count(0), collected_name() {}

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
    virtual void startUnit(const char * localname, const char * prefix, const char * URI,
                           int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                           const struct srcsax_attribute * attributes) {

        if(text != "") process_characters();

        std::string full_name = "";

        if(prefix) {

            full_name += prefix;
            full_name += ":";

        }

        full_name += localname;

        profile_stack.push_back(full_name);

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
    virtual void startElement(const char * localname, const char * prefix, const char * URI,
                                int num_namespaces, const struct srcsax_namespace * namespaces, int num_attributes,
                                const struct srcsax_attribute * attributes) {

        const std::string local_name(localname);

        if(text != "") process_characters();

        if(strcmp(URI, "http://www.sdml.info/srcDiff") == 0) {

            /* @todo check if move and put as in common */
            bool is_change = false;
            bool is_move = false;
            for(int i = 0; i < num_attributes; ++i) {

                if(strcmp(attributes[i].localname, "type") == 0) {

                    std::string value = attributes[i].value;
                    if(value == "change") is_change = true;

                } else if(strcmp(attributes[i].localname, "move") == 0) {

                    is_move = true;

                }

            }

            if(local_name == "common" || is_move)
                srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, is_change, is_move));
            else if(local_name == "delete")
                srcdiff_stack.push_back(srcdiff(SRCDIFF_DELETE, is_change, is_move));
            else if(local_name == "insert")
                srcdiff_stack.push_back(srcdiff(SRCDIFF_INSERT, is_change, is_move));

            if((srcdiff_stack.back().operation == SRCDIFF_DELETE || srcdiff_stack.back().operation == SRCDIFF_INSERT)
               && srcdiff_stack.at(srcdiff_stack.size() - 2).operation == SRCDIFF_COMMON)
                profile_stack.back().is_modified = true;

        }

        std::string full_name = "";

        if(prefix) {

            full_name += prefix;
            full_name += ":";

        }

        full_name += local_name;

        profile_stack.push_back(profile_t(full_name));

        if(strcmp(URI, "http://www.sdml.info/srcDiff") != 0) {

            if(local_name == "name") {

                ++name_count;

            }

            ++srcdiff_stack.back().level;

            if(is_count(full_name)) {

                counting_profile_pos.push_back(profile_stack.size() - 1);
                profile_stack.back().set_id();

            }

            if(srcdiff_stack.back().operation != SRCDIFF_COMMON) {

                if(srcdiff_stack.back().level == 1) {

                    if(local_name == "comment") {

                        total.inc_comment();
                        profile_stack.at(profile_stack.size() - 3).is_comment = true;
                        ++profile_stack.at(profile_stack.size() - 3).comment_count;
                        ++profile_stack.at(profile_stack.size() - 3).total_count;

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
                        profile_stack.at(profile_stack.size() - 3).is_syntax = true;
                        ++profile_stack.at(profile_stack.size() - 3).syntax_count;
                        ++profile_stack.at(profile_stack.size() - 3).total_count;

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

                } else if(srcdiff_stack.back().level == 2 && profile_stack.at(profile_stack.size() - 2).type_name == "template"
                     && (is_funct_type(full_name) || is_class_type(full_name) || is_decl_stmt(full_name))) {

                    count_diff(false);

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
    virtual void endRoot(const char * localname, const char * prefix, const char * URI) {

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
    virtual void endUnit(const char * localname, const char * prefix, const char * URI) {

        if(text != "") process_characters();

        profile_stack.pop_back();

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
    virtual void endElement(const char * localname, const char * prefix, const char * URI) {

        const std::string local_name(localname);

        if(text != "") process_characters();

        if(strcmp(URI, "http://www.sdml.info/srcDiff") == 0) {

            if(local_name == "common" || local_name == "delete" || local_name == "insert")
                srcdiff_stack.pop_back();

        }

        std::string full_name = "";

        if(prefix) {

            full_name += prefix;
            full_name += ":";

        }

        full_name += local_name;


        if(strcmp(URI, "http://www.sdml.info/srcDiff") != 0) {

            if(local_name == "name") {

                --name_count;

                if(name_count == 0) {

                    profile_stack.at(profile_stack.size() - 2).name = collected_name;
                    collected_name = "";

                }

            }

            --srcdiff_stack.back().level;

        if(profile_stack.back().has_assignment
           && profile_stack.at(profile_stack.size() - 2).type_name != "diff:delete"
           && profile_stack.at(profile_stack.size() - 2).type_name != "diff:insert"
           && profile_stack.at(profile_stack.size() - 2).type_name != "diff:common")
        profile_stack.at(profile_stack.size() - 2).has_assignment = true;

            if(profile_stack.back().is_modified) {

                count_modified();

                if(profile_stack.at(profile_stack.size() - 2).type_name != "diff:delete"
                   && profile_stack.at(profile_stack.size() - 2).type_name != "diff:insert"
                   && profile_stack.at(profile_stack.size() - 2).type_name != "diff:common") {

                    profile_stack.at(profile_stack.size() - 2).is_modified = true;
                    profile_stack.at(profile_stack.size() - 2).modified_count += profile_stack.back().modified_count;
                    profile_stack.at(profile_stack.size() - 2).total_count += profile_stack.back().total_count;

                    if(profile_stack.back().is_whitespace) {

                        profile_stack.at(profile_stack.size() - 2).is_whitespace = true;
                        profile_stack.at(profile_stack.size() - 2).whitespace_count += profile_stack.back().whitespace_count;

                    }

                    if(profile_stack.back().is_comment) {

                        profile_stack.at(profile_stack.size() - 2).is_comment = true;
                        profile_stack.at(profile_stack.size() - 2).comment_count += profile_stack.back().comment_count;

                    }

                    if(profile_stack.back().is_syntax) {

                        profile_stack.at(profile_stack.size() - 2).is_syntax = true;
                        profile_stack.at(profile_stack.size() - 2).syntax_count += profile_stack.back().syntax_count;

                    }

                }

            }

            if(is_count(full_name)) {

                counting_profile_pos.pop_back();

                if(profile_list.size() < profile_stack.back().id)
                    profile_list.resize(profile_stack.back().id * 2);

                profile_list[profile_stack.back().id] = profile_stack.back();

                if(counting_profile_pos.size() > 0)
                    profile_stack.at(counting_profile_pos.back()).child_profiles.push_back(profile_stack.back().id);

            }

        }

        profile_stack.pop_back();

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

        text.append(ch, len);

        if(name_count) collected_name.append(ch, len);

    }

};

#endif
