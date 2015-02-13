#include <srcdiff_summary_handler.hpp>

#include <type_query.hpp>

#include <unit_profile_t.hpp>
#include <function_profile_t.hpp>
#include <parameter_profile_t.hpp>

bool is_count(const std::string & type_name) {

return is_function_type(type_name) || is_class_type(type_name) || is_simple_type(type_name)
    || is_condition_type(type_name) || is_catch(type_name) || is_decl_stmt(type_name)
    || is_call(type_name) || is_preprocessor_special(type_name) || is_expr(type_name)
    || is_template(type_name) || is_parameter(type_name) || is_lambda(type_name);

}

bool is_summary(const std::string & type_name) {

    return is_class_type(type_name) || is_function_type(type_name);

}

std::shared_ptr<profile_t> make_profile(const std::string & type_name, srcdiff_type operation) {

    if(is_function_type(type_name)) return std::make_shared<function_profile_t>(type_name, operation);
    if(is_parameter(type_name))     return std::make_shared<parameter_profile_t>(type_name, operation);

    return std::make_shared<profile_t>(type_name, operation);

}

void srcdiff_summary_handler::update_diff_map(std::map<std::string, counts_t> & map, const std::string & name, bool is_whitespace) {

    if(map.find(name) == map.end())
        map[name] = counts_t();

    map[name].inc_total();

    if(is_whitespace)
        map[name].inc_whitespace();
    else
        map[name].inc_syntax();


}

void srcdiff_summary_handler::update_diff(const std::string & name, size_t profile_pos, bool is_whitespace) {


    if(srcdiff_stack.back().operation == SRCDIFF_INSERT) {

        // global lists
        update_diff_map(inserted, name, is_whitespace);

        // profile lists
        update_diff_map(profile_stack.at(profile_pos)->inserted, name, is_whitespace);


    }

    else {

        // global lists
        update_diff_map(deleted, name, is_whitespace);

        // profile lists
        update_diff_map(profile_stack.at(profile_pos)->deleted, name, is_whitespace);

    }

}

void srcdiff_summary_handler::count_diff(bool is_whitespace) {

    if(counting_profile_pos.empty())
        return;

    for(size_t i = 1; i < counting_profile_pos.size(); ++i) {

        size_t pos = counting_profile_pos.at(i).first;
        size_t summary_pos = counting_profile_pos.at(i - 1).second;

        const std::string & name = profile_stack.at(pos)->type_name;

    	if(is_expr(name)) continue;

        if(pos == (profile_stack.size() - 1)) update_diff(name, summary_pos, is_whitespace);

        else {

            if(is_function_type(name)) {

                if((pos + 1) == (profile_stack.size() - 1) && (profile_stack.back()->type_name == "template"))
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "parameter_list" && profile_stack.back()->type_name == "parameter")
                    update_diff(name + "/parameter", summary_pos, is_whitespace);
                else if((pos + 2) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "parameter_list" && profile_stack.at(pos + 2)->type_name == "parameter"
                        && profile_stack.back()->type_name == "init")
                    update_diff(name + "/parameter/init", summary_pos, is_whitespace);
                else if(profile_stack.back()->type_name == "return")
                    update_diff(name + "/return", summary_pos, is_whitespace);
                else if((pos + 2) == (profile_stack.size() - 1) && (profile_stack.back()->type_name == "member_list"))
                    update_diff(name + "/member_list", summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "member_list"
            && (pos + 3) == (profile_stack.size() - 1) && profile_stack.back()->type_name == "call")
                    update_diff(name + "/member_list/call", summary_pos, is_whitespace);

            } else if(is_class_type(name)) {

                if((pos + 2) == (profile_stack.size() - 1)
                   && (profile_stack.back()->type_name == "decl"
                       || profile_stack.back()->type_name == "super"
                       || profile_stack.back()->type_name == "template"))
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "block"
                        && (pos + 3) == (profile_stack.size() - 1) 
            && (profile_stack.back()->type_name == "private" || profile_stack.back()->type_name == "public"
                            || profile_stack.back()->type_name == "protected" || profile_stack.back()->type_name == "signals"))
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "block"
                        && ((pos + 3) == (profile_stack.size() - 1) 
                        || ((pos + 4) == (profile_stack.size() - 1) 
                        && (profile_stack.at(pos + 2)->type_name == "private" || profile_stack.at(pos + 2)->type_name == "public"
                        || profile_stack.at(pos + 2)->type_name == "protected" || profile_stack.at(pos + 2)->type_name == "signals")))
                        && (profile_stack.back()->type_name == "decl_stmt" || profile_stack.back()->type_name == "template"
                        || is_function_type(profile_stack.back()->type_name) || is_class_type(profile_stack.back()->type_name))) {

                            if(profile_stack.back()->type_name == "template")
                                update_diff(name + "/block/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                            else
                                update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                    
                } else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "block" && profile_stack.at(profile_stack.size() - 2)->type_name == "template") {

                    int count_template = 1;
                    while(((count_template + 2) < profile_stack.size()) && profile_stack.at((profile_stack.size() - count_template) - 2)->type_name == "template")
                        ++count_template;

                    if(((pos + count_template + 3) == (profile_stack.size() - 1) 
                        || ((pos + count_template + 4) == (profile_stack.size() - 1) 
                        && (profile_stack.at(pos + 2)->type_name == "private" || profile_stack.at(pos + 2)->type_name == "public"
                        || profile_stack.at(pos + 2)->type_name == "protected" || profile_stack.at(pos + 2)->type_name == "signals")))
                        && (is_function_type(profile_stack.back()->type_name) || is_class_type(profile_stack.back()->type_name)
                             || is_decl_stmt(profile_stack.back()->type_name))) {

                            update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                            update_diff(name + "/block/template/" + profile_stack.back()->type_name, summary_pos, is_whitespace);

                    }

                }

            } else if(is_template(name)) {

                if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "parameter_list" && profile_stack.back()->type_name == "parameter")
                    update_diff(name + "/parameter", summary_pos, is_whitespace);
                else if((pos + 1) == (profile_stack.size() - 1) && (is_function_type(profile_stack.back()->type_name) || is_class_type(profile_stack.back()->type_name)
                     || is_decl_stmt(profile_stack.back()->type_name)))
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);

            } else if(is_condition_type(name)) {

            if((pos + 2) == (profile_stack.size() - 1)
                    && (profile_stack.back()->type_name == "incr"
                        || profile_stack.back()->type_name == "case"
                        || profile_stack.back()->type_name == "default"))
                update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
            else if((pos + 3) == (profile_stack.size() - 1)
                    && (profile_stack.back()->type_name == "case"
                        || profile_stack.back()->type_name == "default"))
                update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);

            } else if(is_catch(name)) {

                if((pos + 2) == (profile_stack.size() - 1)
                   && profile_stack.back()->type_name == "parameter")
                    update_diff(name + "/parameter", summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size())
                   && profile_stack.at(pos + 1)->type_name == "parameter_list"
                   && (pos + 3) == (profile_stack.size() - 1)
                   && profile_stack.back()->type_name == "parameter")
                    update_diff(name + "/parameter", summary_pos, is_whitespace);


            } else if(is_decl_stmt(name)) {

                if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "decl" && (pos + 3) == (profile_stack.size() - 1)
                   && (profile_stack.back()->type_name == "init" || profile_stack.back()->type_name == "name" || profile_stack.back()->type_name == "template"))
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                else if(profile_stack.back()->type_name == "decl")
                    update_diff(name + "/decl", summary_pos, is_whitespace);

            } else if(is_call(name)) {

                if((pos + 3) == (profile_stack.size() - 1)
                   && profile_stack.back()->type_name == "argument")
                    update_diff(name + "/argument", summary_pos, is_whitespace);

            } else if(is_preprocessor_special(name)) {

                if((pos + 2) == (profile_stack.size() - 1)
                   && (profile_stack.back()->type_name == "name"
                       || profile_stack.back()->type_name == "cpp:macro"
                       || profile_stack.back()->type_name == "cpp:value"
                       || profile_stack.back()->type_name == "cpp:number"
                       || profile_stack.back()->type_name == "cpp:file"))
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "cpp:macro"
                   && (pos + 3) == (profile_stack.size() - 1)
                   && profile_stack.back()->type_name == "parameter_list")
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);
                else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "cpp:macro" 
            && profile_stack.at(pos + 2)->type_name == "parameter_list"
            && (pos + 4) == (profile_stack.size() - 1)
            && profile_stack.back()->type_name == "parameter")
                    update_diff(name + "/" + profile_stack.back()->type_name, summary_pos, is_whitespace);

            }

        }

    }

}

void srcdiff_summary_handler::update_modified_map(std::map<std::string, counts_t> & map, const std::string & name) {

    if(map.find(name) == map.end())
        map[name] = counts_t();

    map[name].inc_total();

    if(profile_stack.back()->is_whitespace) map[name].inc_whitespace();

    if(profile_stack.back()->is_comment) map[name].inc_comment();

    if(profile_stack.back()->is_syntax) map[name].inc_syntax();

}

void srcdiff_summary_handler::update_modified_map_summary(std::map<std::string, counts_t> & map, const std::string & name) {

    if(map.find(name) == map.end())
        map[name] = counts_t();

    map[name].total += profile_stack.back()->total_count;

    if(profile_stack.back()->is_whitespace) map[name].whitespace += profile_stack.back()->whitespace_count;

    if(profile_stack.back()->is_comment) map[name].comment += profile_stack.back()->comment_count;

    if(profile_stack.back()->is_syntax) map[name].syntax += profile_stack.back()->syntax_count;

}

void srcdiff_summary_handler::update_modified(const std::string & name, size_t profile_pos) {

    update_modified_map(modified, name);

    if(profile_pos != (profile_stack.size() - 1))
        update_modified_map_summary(profile_stack.at(profile_pos)->modified, name);

}

void srcdiff_summary_handler::count_modified() {

    if(counting_profile_pos.empty() || srcdiff_stack.back().operation != SRCDIFF_COMMON)
        return;

    for(int i = 0; i < counting_profile_pos.size(); ++i) {

        size_t pos = counting_profile_pos.at(i).first;
        size_t summary_pos = counting_profile_pos.at(i).second;
        std::string name = profile_stack.at(pos)->type_name;

        if(pos == (profile_stack.size() - 1)) {

        if(!is_expr(name)) update_modified(name, summary_pos);
        else if(profile_stack.back()->has_assignment) update_modified(name + "/assignment", summary_pos);

        } else if(is_function_type(name)) {

            if((pos + 1) == (profile_stack.size() - 1)
               && (profile_stack.at(pos + 1)->type_name == "type"
                   || profile_stack.at(pos + 1)->type_name == "name"
                   || profile_stack.at(pos + 1)->type_name == "parameter_list"
                   || profile_stack.at(pos + 1)->type_name == "block"
                   || profile_stack.at(pos + 1)->type_name == "member_list"
                   || profile_stack.at(pos + 1)->type_name == "template"))
                update_modified(name + "/" + profile_stack.at(pos + 1)->type_name, summary_pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "parameter_list"
                    && (pos + 2) == (profile_stack.size() - 1)
                      && profile_stack.back()->type_name == "parameter")
                update_modified(name + "/parameter", summary_pos);
            else if((pos + 2) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "parameter_list"
                    && profile_stack.at(pos + 2)->type_name == "parameter"
                    && profile_stack.back()->type_name == "init")
                update_modified(name + "/parameter/init", summary_pos);
            else if(profile_stack.back()->type_name == "return")
                update_modified(name + "/return", summary_pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "member_list"
                    && (pos + 2) == (profile_stack.size() - 1)
            && profile_stack.back()->type_name == "call")
                update_modified(name + "/member_list/call", summary_pos);

        } else if(is_class_type(name)) {

            if((profile_stack.size() - 1) == (pos + 1)
               && (profile_stack.back()->type_name == "super"
                   || profile_stack.back()->type_name == "name"
                   || profile_stack.back()->type_name == "block"
                   || profile_stack.back()->type_name == "decl"
                   || profile_stack.back()->type_name == "template"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
        else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "block"
            && (pos + 2) == (profile_stack.size() - 1)
            && (profile_stack.back()->type_name == "private" || profile_stack.back()->type_name == "public"
            || profile_stack.back()->type_name == "protected" || profile_stack.back()->type_name == "signals"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "block"
                    && ((pos + 2) == (profile_stack.size() - 1) 
            || ((pos + 3) == (profile_stack.size() - 1) 
                && (profile_stack.at(pos + 2)->type_name == "private" || profile_stack.at(pos + 2)->type_name == "public"
                || profile_stack.at(pos + 2)->type_name == "protected" || profile_stack.at(pos + 2)->type_name == "signals")))
                    && (profile_stack.back()->type_name == "decl_stmt" || profile_stack.back()->type_name == "template"
                        || is_function_type(profile_stack.back()->type_name) || is_class_type(profile_stack.back()->type_name))) {

                        if(profile_stack.back()->type_name == "template")
                            update_modified(name + "/block/" + profile_stack.back()->type_name, summary_pos);
                        else
                            update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);

            } else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "block" && profile_stack.at(profile_stack.size() - 2)->type_name == "template")  {

                    int count_template = 1;
                    while(((count_template + 2) < profile_stack.size()) && profile_stack.at((profile_stack.size() - count_template) - 2)->type_name == "template")
                        ++count_template;

                    if(((pos + count_template + 2) == (profile_stack.size() - 1) 
                        || ((pos + count_template + 3) == (profile_stack.size() - 1) 
                        && (profile_stack.at(pos + 2)->type_name == "private" || profile_stack.at(pos + 2)->type_name == "public"
                        || profile_stack.at(pos + 2)->type_name == "protected" || profile_stack.at(pos + 2)->type_name == "signals")))
                        && (is_function_type(profile_stack.back()->type_name) || is_class_type(profile_stack.back()->type_name)
                            || is_decl_stmt(profile_stack.back()->type_name))) {

                            update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
                            update_modified(name + "/block/template/" + profile_stack.back()->type_name, summary_pos);

                    }

                }

        } else if(is_template(name)) { 

            if((pos + 1) == (profile_stack.size() - 1) && profile_stack.back()->type_name == "parameter_list")
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 1) == (profile_stack.size() - 1) && (is_function_type(profile_stack.back()->type_name) || is_class_type(profile_stack.back()->type_name)
                || is_decl_stmt(profile_stack.back()->type_name)))
                    update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "parameter_list"
                      && profile_stack.back()->type_name == "parameter")
                update_modified(name + "/parameter", summary_pos);

        } else if(is_condition_type(name)) {

            if((pos + 1) == (profile_stack.size() - 1)
               && (profile_stack.back()->type_name == "init"
                   || profile_stack.back()->type_name == "condition"
                   || profile_stack.back()->type_name == "incr"
                   || profile_stack.back()->type_name == "then"
                   || profile_stack.back()->type_name == "case"
                   || profile_stack.back()->type_name == "default"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 2) == (profile_stack.size() - 1)
                   && (profile_stack.back()->type_name == "case"
                   || profile_stack.back()->type_name == "default"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);

        } else if(is_catch(name)) {

            if((pos + 1) == (profile_stack.size() - 1)
               && (profile_stack.back()->type_name == "parameter_list"
                   || profile_stack.back()->type_name == "parameter"
                   || profile_stack.back()->type_name == "block"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 2) < (profile_stack.size())
                   && profile_stack.at(pos + 1)->type_name == "parameter_list"
                   && profile_stack.back()->type_name == "parameter")
                update_modified(name + "/parameter", summary_pos);

        } else if(is_decl_stmt(name)) {

            if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "decl"
               && (pos + 2) == (profile_stack.size() - 1)
               && (profile_stack.back()->type_name == "type"
                   || profile_stack.back()->type_name == "name"
                   || profile_stack.back()->type_name == "init"
                   || profile_stack.back()->type_name == "template"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);

        } else if(is_call(name)) {

            if((pos + 1) == (profile_stack.size() - 1)
               && (profile_stack.at(pos + 1)->type_name == "name"
                   || profile_stack.back()->type_name == "argument_list"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "argument_list"
               && (pos + 2) == (profile_stack.size() - 1)
               && profile_stack.back()->type_name == "argument")
                update_modified(name + "/argument", summary_pos);

        } else if(is_preprocessor_special(name)) {

        if((pos + 1) == (profile_stack.size() - 1)
           && (profile_stack.back()->type_name == "name"
                   || profile_stack.back()->type_name == "cpp:macro"
                   || profile_stack.back()->type_name == "cpp:value"
                   || profile_stack.back()->type_name == "cpp:number"
                   || profile_stack.back()->type_name == "cpp:file"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 1) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "cpp:macro"
               && (pos + 2) == (profile_stack.size() - 1)
               && (profile_stack.back()->type_name == "parameter_list"
           || profile_stack.back()->type_name == "name"))
                update_modified(name + "/" + profile_stack.back()->type_name, summary_pos);
            else if((pos + 2) < (profile_stack.size()) && profile_stack.at(pos + 1)->type_name == "cpp:macro"
           && profile_stack.at(pos + 2)->type_name == "parameter_list"
               && (pos + 3) == (profile_stack.size() - 1)
               && profile_stack.back()->type_name == "parameter")
                update_modified(name + "/parameter", summary_pos);

        }

    }

}

void srcdiff_summary_handler::process_characters() {

   if(profile_stack.back()->type_name == "op:operator") {

    if(text == "=" || text == "+=" || text == "-=" || text == "*=" || text == "/=" || text == ">>=" || text == "<<=" || text == "%=" || text == "&=" || text == "|=" || text == "^=") {

        profile_stack.back()->has_assignment = true;
        ++profile_stack.back()->assignment_count;

    } else
        profile_stack.back()->has_assignment = false;
        profile_stack.back()->assignment_count = 0;

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

            profile_stack.push_back(make_profile("text", srcdiff_stack.back().operation));

            count_diff(is_whitespace);

            profile_stack.pop_back();

        }


    }

    text = "";

}

srcdiff_summary_handler::srcdiff_summary_handler(profile_t::profile_list_t & profile_list) : id_count(0), profile_list(profile_list), srcdiff_stack(), profile_stack(), counting_profile_pos(),
    inserted(), deleted(), modified(), insert_count(), delete_count(), change_count(), total(), text(),
    name_count(0), collected_name() {}

/**
 * startDocument
 *
 * SAX handler function for start of document.
 * Overide for desired behaviour.
 */
void srcdiff_summary_handler::startDocument() {

    srcdiff_stack.push_back(srcdiff(SRCDIFF_COMMON, false, false));

}

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

    if(text != "") process_characters();

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += localname;

    profile_stack.push_back(std::make_shared<unit_profile_t>(full_name, SRCDIFF_COMMON));

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
            profile_stack.back()->is_modified = true;

    }

    std::string full_name = "";

    if(prefix) {

        full_name += prefix;
        full_name += ":";

    }

    full_name += local_name;

    profile_stack.push_back(make_profile(full_name, srcdiff_stack.back().operation));

    if(strcmp(URI, "http://www.sdml.info/srcDiff") != 0) {

        if(local_name == "name") {

            ++name_count;

        }

        ++srcdiff_stack.back().level;

        if(is_count(full_name)) {


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

                count_diff(false);

            } else if(srcdiff_stack.back().level == 2 && profile_stack.at(profile_stack.size() - 2)->type_name == "template"
                 && (is_function_type(full_name) || is_class_type(full_name) || is_decl_stmt(full_name))) {

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

                profile_stack.at(counting_profile_pos.back().first)->set_name(collected_name, profile_stack.at(profile_stack.size() - 2)->type_name);
                collected_name.reset();

            }

        }

        --srcdiff_stack.back().level;

    if(profile_stack.back()->has_assignment
       && profile_stack.at(profile_stack.size() - 2)->type_name != "diff:delete"
       && profile_stack.at(profile_stack.size() - 2)->type_name != "diff:insert"
       && profile_stack.at(profile_stack.size() - 2)->type_name != "diff:common")
        profile_stack.at(profile_stack.size() - 2)->has_assignment = true;

        if(profile_stack.back()->is_modified) {

            count_modified();

            if(profile_stack.at(profile_stack.size() - 2)->type_name != "diff:delete"
               && profile_stack.at(profile_stack.size() - 2)->type_name != "diff:insert"
               && profile_stack.at(profile_stack.size() - 2)->type_name != "diff:common") {

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

        }

        if(is_count(full_name)) {

            counting_profile_pos.pop_back();

            // do not save items with no changes and not inserted/deleted
            if(profile_stack.back()->total_count || srcdiff_stack.back().operation != SRCDIFF_COMMON) {

	            if(profile_list.size() < profile_stack.back()->id)
	                profile_list.resize(profile_stack.back()->id * 2);

	            profile_list[profile_stack.back()->id] = profile_stack.back();

	            // should always have at least unit
                profile_stack.at(counting_profile_pos.back().second)->add_child(profile_stack.back());

        	}

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
